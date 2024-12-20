#include "MultiprocessTraffic.h"
#include "Reports.h"
#include "TelnetRelayController.h"
#include <chrono>
#include <csignal>
#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

MultiprocessTraffic* MultiprocessTraffic::instance = nullptr;

MultiprocessTraffic::MultiprocessTraffic(const std::string& configFile,
                                         bool debug,
                                         bool verbose)
    : configFile(configFile)
    , debug(debug)
    , verbose(verbose)
{
    instance = this;
    loadJunctionConfig();

    TelnetRelayController::getInstance(
        relayUrl, relayUsername, relayPassword, phases, verbose);

    Reports::getInstance(
        httpUrl, tSecretKey, junctionId, junctionName, verbose);

    std::signal(SIGINT, MultiprocessTraffic::handleSignal);
    std::signal(SIGCHLD, MultiprocessTraffic::handleSignal);
}

void MultiprocessTraffic::start()
{
    createPipes();
    forkChildren();

    ParentProcess parentProcess(numVehicle,
                                numPedestrian,
                                pipesParentToChild,
                                pipesChildToParent,
                                phases,
                                phaseDurations,
                                verbose,
                                densityMultiplierGreenPhase,
                                densityMultiplierRedPhase,
                                densityMin,
                                densityMax,
                                minPhaseDurationMs,
                                minPedestrianDurationMs,
                                relayUrl,
                                subLocationId,
                                junctionId,
                                junctionName);

    parentProcess.run();
}

void MultiprocessTraffic::handleSignal(int signal)
{
    if(instance == nullptr)
    {
        std::cerr << "MultiprocessTraffic instance is null\n";
        exit(EXIT_FAILURE);
    }

    if(signal == SIGINT)
    {
        TelnetRelayController& telnetRelay =
            TelnetRelayController::getInstance();

        // Turn off all relays before exiting
        telnetRelay.turnOffAllRelay();

        std::cout << "\nInterrupt signal received. Turning off all relays...\n";
        for(pid_t pid : instance->childPids)
        {
            std::cout << "Killing Child PID: " << pid << "\n";
            kill(pid, SIGTERM);
        }
        std::cout << "Exiting Parent PID: " << getpid() << "\n";
        exit(EXIT_SUCCESS);
    }

    if(signal == SIGCHLD)
    {
        static std::mutex standbyMutex;

        pid_t pid;
        while((pid = waitpid(-1, nullptr, WNOHANG)) > 0)
        {
            std::cout << "Reaped Child PID: " << pid << "\n";
            // Remove the terminated child PID from the child list
            instance->childPids.erase(std::remove(instance->childPids.begin(),
                                                  instance->childPids.end(),
                                                  pid),
                                      instance->childPids.end());
        }

        // Ensure that the standby mode logic is not entered concurrently
        std::lock_guard<std::mutex> lock(standbyMutex);

        TelnetRelayController& telnetRelay =
            TelnetRelayController::getInstance();

        // Enter standby mode, flashing yellow relay for standbyDuration before exiting
        telnetRelay.setStandbyMode(instance->yellowChannels,
                                   instance->standbyDuration);

        std::cout << "\nOne of the children unexpectedly crashed.\n";
        // Kill remaining child processes
        for(pid_t pid : instance->childPids)
        {
            std::cout << "Killing Child PID: " << pid << "\n";
            if(kill(pid, SIGTERM) == -1)
            {
                std::cerr << "Failed to kill child PID: " << pid
                          << ", error: " << strerror(errno) << "\n";
            }
        }

        std::cout << "Exiting Parent PID: " << getpid() << "\n";
        exit(EXIT_SUCCESS);
    }
}

void MultiprocessTraffic::createPipes()
{
    pipesParentToChild.resize(numChildren);
    pipesChildToParent.resize(numChildren);

    for(int i = 0; i < numChildren; ++i)
    {
        if(pipe(pipesParentToChild[i].fds) == -1 ||
           pipe(pipesChildToParent[i].fds) == -1)
        {
            std::cerr << "Pipe creation failed: " << strerror(errno) << "\n";
            exit(EXIT_FAILURE);
        }
    }
}

void MultiprocessTraffic::forkChildren()
{
    int pipeIndex = 0;

    if(verbose)
    {
        std::cout << "Parent PID: " << getpid() << "\n";
    }

    for(int i = 0; i < numVehicle; ++i)
    {
        pid_t pid = fork();
        if(pid < 0)
        {
            std::cerr << "Fork failed: " << strerror(errno) << "\n";
            exit(EXIT_FAILURE);
        }
        else if(pid == 0)
        {
            ChildProcess vehicleProcess(pipeIndex,
                                        pipesParentToChild[pipeIndex],
                                        pipesChildToParent[pipeIndex],
                                        verbose);
            vehicleProcess.runVehicle(
                debug, streamConfigs[pipeIndex], streamLinks[pipeIndex]);
            exit(EXIT_SUCCESS);
        }
        else
        {
            childPids.push_back(pid);
            if(verbose)
            {
                std::cout << "Vehicle Child " << pipeIndex << " PID: " << pid
                          << "\n";
            }
        }
        ++pipeIndex;
    }

    for(int i = 0; i < numPedestrian; ++i)
    {
        pid_t pid = fork();
        if(pid < 0)
        {
            std::cerr << "Fork failed: " << strerror(errno) << "\n";
            exit(EXIT_FAILURE);
        }
        else if(pid == 0)
        {
            ChildProcess pedestrianProcess(pipeIndex,
                                           pipesParentToChild[pipeIndex],
                                           pipesChildToParent[pipeIndex],
                                           verbose);
            pedestrianProcess.runPedestrian(
                debug, streamConfigs[pipeIndex], streamLinks[pipeIndex]);
            exit(EXIT_SUCCESS);
        }
        else
        {
            childPids.push_back(pid);
            if(verbose)
            {
                std::cout << "Pedestrian Child " << pipeIndex << " PID: " << pid
                          << "\n";
            }
        }
        ++pipeIndex;
    }
}

void MultiprocessTraffic::calibrate()
{
    WatcherSpawner spawner;

    for(int i = 0; i < numChildren; ++i)
    {
        // calibration can only be done with GUI, not headless
        Watcher* calibrateWatcherGui =
            spawner.spawnWatcher(WatcherType::CALIBRATE,
                                 RenderMode::GUI,
                                 streamLinks[i],
                                 streamConfigs[i]);
        delete calibrateWatcherGui;
    }
}

void MultiprocessTraffic::loadJunctionConfig()
{
    YAML::Node config = YAML::LoadFile(configFile);

    loadJunctionInfo(config);
    loadPhases(config);
    loadPhaseDurations(config);
    loadDensitySettings(config);
    loadStreamInfo(config);
    loadRelayInfo(config);
    loadHttpInfo(config);

    setVehicleAndPedestrianCount();
    setYellowChannels(config);
}

void MultiprocessTraffic::loadJunctionInfo(const YAML::Node& config)
{

    if(!config["subLocationId"] || !config["junctionId"] ||
       !config["junctionName"])
    {
        std::cerr
            << "Error: Missing junction information in configuration file!"
            << std::endl;
        exit(EXIT_FAILURE);
    }

    subLocationId = config["subLocationId"].as<int>();
    junctionId = config["junctionId"].as<int>();
    junctionName = config["junctionName"].as<std::string>();
}

void MultiprocessTraffic::loadHttpInfo(const YAML::Node& config)
{
    if(config["httpUrl"] && config["httpUrl"].IsScalar())
    {
        httpUrl = config["httpUrl"].as<std::string>();
    }
    else
    {
        httpUrl = "https://55qdnlqk-5234.asse.devtunnels.ms"; // Default value
        std::cerr << "Warning: httpUrl not provided, using default."
                  << std::endl;
    }

    if(config["tSecretKey"] && config["httpUrl"].IsScalar())
    {
        tSecretKey = config["tSecretKey"].as<std::string>();
    }
    else
    {
        tSecretKey = "TrafficEz-001-002-003-004"; // Default value
        std::cerr << "Warning: tSecretKey not provided, using default."
                  << std::endl;
    }
}

void MultiprocessTraffic::loadPhases(const YAML::Node& config)
{
    if(!config["phases"])
    {
        std::cerr << "Error: No phases config found!\n";
        exit(EXIT_FAILURE);
    }

    phases.clear();
    for(const auto& phase : config["phases"])
    {
        std::vector<PhaseMessageType> phaseVector;
        for(const auto& phaseStr : phase)
        {
            std::string phaseString = phaseStr.as<std::string>();
            PhaseMessageType phaseType = getPhaseMessageType(phaseString);
            phaseVector.push_back(phaseType);
        }
        phases.push_back(phaseVector);
    }
}

void MultiprocessTraffic::loadPhaseDurations(const YAML::Node& config)
{
    if(!config["phaseDurations"])
    {
        std::cerr << "Error: No phaseDurations config found!\n";
        exit(EXIT_FAILURE);
    }

    phaseDurations.clear();
    for(const auto& duration : config["phaseDurations"])
    {
        phaseDurations.push_back(duration.as<int>());
    }

    if(phases.size() != phaseDurations.size())
    {
        std::cerr << "Size of phase info and duration do not match!\n";
        exit(EXIT_FAILURE);
    }

    if(config["standbyDuration"])
    {
        standbyDuration = config["standbyDuration"].as<int>();
    }
    else
    {
        standbyDuration = 60000; // Default value in milliseconds
        std::cerr << "Warning: standbyDuration not provided, using default: "
                  << standbyDuration << std::endl;
    }
}

void MultiprocessTraffic::loadDensitySettings(const YAML::Node& config)
{
    if(!config["densityMultiplierGreenPhase"] ||
       !config["densityMultiplierRedPhase"] || !config["densityMin"] ||
       !config["densityMax"] || !config["minPhaseDurationMs"] ||
       !config["minPedestrianDurationMs"])
    {
        std::cerr << "Error: Missing density settings in configuration file!\n";
        exit(EXIT_FAILURE);
    }

    densityMultiplierGreenPhase =
        config["densityMultiplierGreenPhase"].as<float>();
    densityMultiplierRedPhase = config["densityMultiplierRedPhase"].as<float>();
    densityMin = config["densityMin"].as<float>();
    densityMax = config["densityMax"].as<float>();
    minPhaseDurationMs = config["minPhaseDurationMs"].as<int>();
    minPedestrianDurationMs = config["minPedestrianDurationMs"].as<int>();
}

void MultiprocessTraffic::loadStreamInfo(const YAML::Node& config)
{
    streamConfigs.clear();
    streamLinks.clear();

    for(const auto& stream : config["streamInfo"])
    {
        if(stream.size() != 2)
        {
            std::cerr << "Error: Invalid streamInfo entry!\n";
            exit(EXIT_FAILURE);
        }

        streamConfigs.push_back(stream[0].as<std::string>());
        streamLinks.push_back(stream[1].as<std::string>());
    }
}

void MultiprocessTraffic::loadRelayInfo(const YAML::Node& config)
{
    if(!config["relayUrl"] || !config["relayUsername"] ||
       !config["relayPassword"])
    {
        std::cerr << "Error: Missing relay info in configuration file!\n";
        exit(EXIT_FAILURE);
    }

    relayUrl = config["relayUrl"].as<std::string>();
    relayUsername = config["relayUsername"].as<std::string>();
    relayPassword = config["relayPassword"].as<std::string>();
}

void MultiprocessTraffic::setVehicleAndPedestrianCount()
{
    numVehicle = 0;
    numPedestrian = 0;
    numChildren = phases[0].size();

    for(const auto& phase : phases[0])
    {
        switch(phase)
        {
        case GREEN_PHASE:
        case RED_PHASE:
            numVehicle++;
            break;
        case GREEN_PED:
        case RED_PED:
            numPedestrian++;
            break;
        default:
            std::cerr << "Unknown phase type!\n";
            exit(EXIT_FAILURE);
        }
    }

    if(numChildren != numVehicle + numPedestrian)
    {
        std::cerr << "Error: Count of children(" << numChildren
                  << ") do not match Vehicle(" << numVehicle
                  << ") + Pedestrian(" << numPedestrian << ")\n";
        exit(EXIT_FAILURE);
    }

    if(numChildren != streamConfigs.size() || numChildren != streamLinks.size())
    {
        std::cerr << "Error: Count of children(" << numChildren
                  << ") do not match streamConfigs(" << streamConfigs.size()
                  << ") or streamLinks(" << streamLinks.size() << ")\n";
        exit(EXIT_FAILURE);
    }
}

int MultiprocessTraffic::calculateTotalChannels(
    const std::vector<std::string>& childrenPhases)
{
    int totalChannels = 0;

    for(const auto& phase : childrenPhases)
    {
        if(phase == "GREEN_PHASE" || phase == "RED_PHASE")
        {
            totalChannels += 3;
        }
        else if(phase == "GREEN_PED" || phase == "RED_PED")
        {
            totalChannels += 2;
        }
    }

    return totalChannels;
}

void MultiprocessTraffic::assignChannels(
    const std::vector<std::string>& childrenPhases,
    std::vector<int>& yellowChannels)
{
    int currentIndex = 0;

    // Loop through each phase and assign channels
    for(const auto& phase : childrenPhases)
    {
        if(phase == "GREEN_PHASE" || phase == "RED_PHASE")
        {
            currentIndex += 3;
            yellowChannels.push_back(
                currentIndex - 3 +
                2); // Yellow is the third channel in each group
        }
        else if(phase == "GREEN_PED" || phase == "RED_PED")
        {
            // Add 2 channels for each GREEN_PED or RED_PED (green, red)
            currentIndex += 2;
        }
    }
}

void MultiprocessTraffic::setYellowChannels(const YAML::Node& config)
{
    if(!config["phases"])
    {
        std::cerr << "Error: No phases config found!\n";
        exit(EXIT_FAILURE);
    }

    std::vector<int> computedYellowChannels;

    for(const auto& phaseSet : config["phases"])
    {
        std::vector<std::string> childrenPhases;
        for(const auto& phase : phaseSet)
        {
            childrenPhases.push_back(phase.as<std::string>());
        }

        std::vector<int> tempYellowChannels;
        assignChannels(childrenPhases, tempYellowChannels);

        if(!computedYellowChannels.empty() &&
           computedYellowChannels != tempYellowChannels)
        {
            std::cerr
                << "Error: Inconsistent yellow channel assignments detected "
                   "across phase sets!\n";
            exit(EXIT_FAILURE);
        }

        if(computedYellowChannels.empty())
        {
            computedYellowChannels = tempYellowChannels;
        }
    }

    yellowChannels = computedYellowChannels;
}

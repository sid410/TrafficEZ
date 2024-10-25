#include "MultiprocessTraffic.h"
#include "TelnetRelayController.h"
#include <csignal>
#include <cstring>
#include <iostream>
#include <sys/wait.h>
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
        std::cout << "\nInterrupt signal received.\n";
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
        std::cout << "\nOne of the children unexpectedly crashed.\n";
        for(pid_t pid : instance->childPids)
        {
            std::cout << "Killing Child PID: " << pid << "\n";
            kill(pid, SIGTERM);
        }
        std::cout << "Exiting Parent PID: " << getpid() << "\n";

        TelnetRelayController& relay = TelnetRelayController::getInstance();
        relay.standbyMode();
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
            ChildProcess childProcess(pipeIndex,
                                      pipesParentToChild[pipeIndex],
                                      pipesChildToParent[pipeIndex],
                                      verbose);
            childProcess.runVehicle(
                debug, streamConfigs[pipeIndex], streamLinks[pipeIndex]);
            exit(EXIT_SUCCESS);
        }
        else
        {
            childPids.push_back(pid);
            if(verbose)
            {
                std::cout << "Child " << pipeIndex << " PID: " << pid << "\n";
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
            ChildProcess childProcess(pipeIndex,
                                      pipesParentToChild[pipeIndex],
                                      pipesChildToParent[pipeIndex],
                                      verbose);
            childProcess.runPedestrian(
                debug, streamConfigs[pipeIndex], streamLinks[pipeIndex]);
            exit(EXIT_SUCCESS);
        }
        else
        {
            childPids.push_back(pid);
            if(verbose)
            {
                std::cout << "Child " << pipeIndex << " PID: " << pid << "\n";
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

    setVehicleAndPedestrianCount();
}

void MultiprocessTraffic::loadJunctionInfo(const YAML::Node& config)
{
    if(!config["junctionId"] || !config["junctionName"])
    {
        std::cerr << "Missing junction information in configuration file!\n";
        exit(EXIT_FAILURE);
    }

    junctionId = config["junctionId"].as<int>();
    junctionName = config["junctionName"].as<std::string>();
}

void MultiprocessTraffic::loadPhases(const YAML::Node& config)
{
    if(!config["phases"])
    {
        std::cerr << "No phases config found!\n";
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
        std::cerr << "No phaseDurations config found!\n";
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
}

void MultiprocessTraffic::loadDensitySettings(const YAML::Node& config)
{
    if(!config["densityMultiplierGreenPhase"] ||
       !config["densityMultiplierRedPhase"] || !config["densityMin"] ||
       !config["densityMax"] || !config["minPhaseDurationMs"] ||
       !config["minPedestrianDurationMs"])
    {
        std::cerr << "Missing density settings in configuration file!\n";
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
            std::cerr << "Invalid streamInfo entry!\n";
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
        std::cerr << "Missing relay info in configuration file!\n";
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
        std::cerr << "Count of children(" << numChildren
                  << ") do not match Vehicle(" << numVehicle
                  << ") + Pedestrian(" << numPedestrian << ")\n";
        exit(EXIT_FAILURE);
    }

    if(numChildren != streamConfigs.size() || numChildren != streamLinks.size())
    {
        std::cerr << "Count of children(" << numChildren
                  << ") do not match streamConfigs(" << streamConfigs.size()
                  << ") or streamLinks(" << streamLinks.size() << ")\n";
        exit(EXIT_FAILURE);
    }
}

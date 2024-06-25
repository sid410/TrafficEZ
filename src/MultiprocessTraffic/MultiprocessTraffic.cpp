#include "MultiprocessTraffic.h"
#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

MultiprocessTraffic::MultiprocessTraffic(const std::string& configFile,
                                         bool verbose)
    : configFile(configFile)
    , verbose(verbose)
{}

void MultiprocessTraffic::start()
{
    loadJunctionConfig();
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
                                minPedestrianDurationMs);
    parentProcess.run();
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
            childProcess.runVehicle(true, i);
            exit(EXIT_SUCCESS);
        }
        else
        {
            childPids.push_back(pid);
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
            childProcess.runPedestrian(true, i);
            exit(EXIT_SUCCESS);
        }
        else
        {
            childPids.push_back(pid);
        }
        ++pipeIndex;
    }
}

void MultiprocessTraffic::loadJunctionConfig()
{
    YAML::Node config = YAML::LoadFile(configFile);

    if(!config["phases"] || !config["phaseDurations"])
    {
        std::cerr << "Invalid configuration file!\n";
        exit(EXIT_FAILURE);
    }

    loadPhases(config);
    loadPhaseDurations(config);
    loadDensitySettings(config);
    setVehicleAndPedestrianCount();
}

void MultiprocessTraffic::loadPhases(const YAML::Node& config)
{
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
}
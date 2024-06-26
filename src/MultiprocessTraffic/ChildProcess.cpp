#include "ChildProcess.h"
#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <unistd.h>

ChildProcess* ChildProcess::instance = nullptr;

ChildProcess::ChildProcess(int childIndex,
                           Pipe& pipeParentToChild,
                           Pipe& pipeChildToParent,
                           bool verbose)
    : childIndex(childIndex)
    , pipeParentToChild(pipeParentToChild)
    , pipeChildToParent(pipeChildToParent)
    , verbose(verbose)
{
    instance = this;
    std::signal(SIGINT, ChildProcess::handleSignal);
    std::signal(SIGCHLD, ChildProcess::handleSignal);

    closeUnusedPipes();
}

void ChildProcess::handleSignal(int signal)
{
    if(instance == nullptr)
    {
        std::cerr << "Child " << instance->childIndex << " instance is null\n";
        exit(EXIT_FAILURE);
    }

    if(signal == SIGTERM)
    {
        std::cout << "Child " << instance->childIndex
                  << " received termination signal. Exiting...\n";
        exit(EXIT_SUCCESS);
    }
}

void ChildProcess::runVehicle(bool debug,
                              const std::string& streamConfig,
                              const std::string& streamLink)
{
    Watcher* vehicleWatcher =
        createWatcher(WatcherType::VEHICLE, debug, streamConfig, streamLink);

    char buffer[BUFFER_SIZE];
    bool isStateGreen = false;

    fcntl(pipeParentToChild.fds[0], F_SETFL, O_NONBLOCK);

    while(true)
    {
        int bytesRead =
            read(pipeParentToChild.fds[0], buffer, sizeof(buffer) - 1);

        processMessageBuffer(bytesRead, buffer, isStateGreen, vehicleWatcher);

        if(isStateGreen)
        {
            vehicleWatcher->processFrame();
        }
        else
        {
            usleep(CPU_SLEEP_US);
        }
    }

    delete vehicleWatcher;
}

void ChildProcess::runPedestrian(bool debug,
                                 const std::string& streamConfig,
                                 const std::string& streamLink)
{
    Watcher* pedestrianWatcher =
        createWatcher(WatcherType::PEDESTRIAN, debug, streamConfig, streamLink);

    char buffer[BUFFER_SIZE];
    bool isStateGreen = false; // just a placeholder, no logic for pedestrian

    fcntl(pipeParentToChild.fds[0], F_SETFL, O_NONBLOCK);

    while(true)
    {
        int bytesRead =
            read(pipeParentToChild.fds[0], buffer, sizeof(buffer) - 1);

        processMessageBuffer(
            bytesRead, buffer, isStateGreen, pedestrianWatcher);

        usleep(CPU_SLEEP_US);
    }

    delete pedestrianWatcher;
}

Watcher* ChildProcess::createWatcher(WatcherType watcherType,
                                     bool debug,
                                     const std::string& streamConfig,
                                     const std::string& streamLink)
{
    if(debug)
    {
        return spawner.spawnWatcher(
            watcherType, RenderMode::GUI, streamLink, streamConfig);
    }
    else
    {
        return spawner.spawnWatcher(
            watcherType, RenderMode::HEADLESS, streamLink, streamConfig);
    }
}

void ChildProcess::processMessageBuffer(int bytesRead,
                                        char* buffer,
                                        bool& isStateGreen,
                                        Watcher* watcher)
{
    if(bytesRead > 0)
    {
        buffer[bytesRead] = '\0'; // Ensure null termination

        if(verbose)
        {
            std::cout << "Child " << childIndex
                      << ": Received phase message: " << buffer << "\n";
        }

        PhaseMessageType phaseType = getPhaseMessageType(buffer);
        handlePhaseMessage(phaseType, watcher, isStateGreen);
    }
}

void ChildProcess::handlePhaseMessage(PhaseMessageType phaseType,
                                      Watcher* watcher,
                                      bool& isStateGreen)
{
    switch(phaseType)
    {

    case RED_PHASE: {
        // send the previous green vehicle density
        float density = watcher->getTrafficDensity();
        sendDensityToParent(density);

        isStateGreen = false;
        watcher->setCurrentTrafficState(TrafficState::RED_PHASE);
        break;
    }

    case GREEN_PHASE: {
        // send the previous red vehicle density
        watcher->processFrame();
        float density = watcher->getTrafficDensity();
        sendDensityToParent(density);

        isStateGreen = true;
        watcher->setCurrentTrafficState(TrafficState::GREEN_PHASE);
        break;
    }

    case RED_PED: {
        // send the waiting pedestrian count
        watcher->processFrame();
        float density = watcher->getInstanceCount();
        sendDensityToParent(density);
        break;
    }

    case GREEN_PED: {
        // ignore the already walking pedestrian
        sendDensityToParent(0.0f);
        break;
    }

    case UNKNOWN:
    default: {
        std::cerr << "Child " << childIndex << ": Unknown message received.\n";
        break;
    }
    }
}

void ChildProcess::sendDensityToParent(float density)
{
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%.2f", density);
    if(write(pipeChildToParent.fds[1], buffer, strlen(buffer) + 1) == -1)
    {
        std::cerr << "Child " << childIndex
                  << ": Failed to write to pipe: " << strerror(errno) << "\n";
    }
}

void ChildProcess::closeUnusedPipes()
{
    close(pipeParentToChild.fds[1]);
    close(pipeChildToParent.fds[0]);
}

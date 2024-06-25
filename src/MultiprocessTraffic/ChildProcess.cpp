#include "ChildProcess.h"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <unistd.h>

ChildProcess::ChildProcess(int childIndex,
                           Pipe& pipeParentToChild,
                           Pipe& pipeChildToParent,
                           bool verbose)
    : childIndex(childIndex)
    , pipeParentToChild(pipeParentToChild)
    , pipeChildToParent(pipeChildToParent)
    , verbose(verbose)
{
    closeUnusedPipes();
}

void ChildProcess::runVehicle(bool debug, int vehicleId)
{
    std::ostringstream configStream, nameStream;
    configStream << "vehicle" << vehicleId << ".yaml";
    nameStream << "stream" << vehicleId << ".mp4";

    Watcher* vehicleWatcher = createWatcher(
        WatcherType::VEHICLE, debug, configStream.str(), nameStream.str());

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

void ChildProcess::runPedestrian(bool debug, int pedestrianId)
{
    std::ostringstream configStream, nameStream;
    configStream << "pedestrian" << pedestrianId << ".yaml";
    nameStream << "streamPed" << pedestrianId << ".mp4";

    Watcher* pedestrianWatcher = createWatcher(
        WatcherType::PEDESTRIAN, debug, configStream.str(), nameStream.str());

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
                                     const std::string& configFile,
                                     const std::string& streamFile)
{
    if(debug)
    {
        return spawner.spawnWatcher(
            watcherType, RenderMode::GUI, streamFile, configFile);
    }
    else
    {
        return spawner.spawnWatcher(
            watcherType, RenderMode::HEADLESS, streamFile, configFile);
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

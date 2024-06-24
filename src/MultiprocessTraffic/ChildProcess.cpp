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

    // read end of the pipe to non-blocking mode
    fcntl(pipeParentToChild.fds[0], F_SETFL, O_NONBLOCK);

    while(true)
    {
        // Check for phase change message from the parent
        int bytesRead =
            read(pipeParentToChild.fds[0], buffer, sizeof(buffer) - 1);
        if(bytesRead > 0)
        {
            buffer[bytesRead] = '\0'; // Ensure null termination

            if(verbose)
            {
                std::cout << "Child " << childIndex
                          << ": Received phase message: " << buffer << "\n";
            }

            if(strcmp(buffer, "RED_PHASE") == 0)
            {
                float density = vehicleWatcher->getTrafficDensity();
                sendDensityToParent(density);

                isStateGreen = false;
                vehicleWatcher->setCurrentTrafficState(TrafficState::RED_PHASE);
            }
            else if(strcmp(buffer, "GREEN_PHASE") == 0)
            {
                // Process 1 red frame first before changing
                vehicleWatcher->processFrame();
                float density = vehicleWatcher->getTrafficDensity();
                sendDensityToParent(density);

                isStateGreen = true;
                vehicleWatcher->setCurrentTrafficState(
                    TrafficState::GREEN_PHASE);
            }
            else
            {
                std::cerr << "Child " << childIndex
                          << ": Unknown message received: " << buffer << "\n";
                break; // Exit if an unknown message is received
            }
        }

        // Process frames continuously if green
        if(isStateGreen)
        {
            vehicleWatcher->processFrame();
        }
        else
        {
            // Sleep for a short period to save CPU resource
            usleep(CPU_SLEEP_US); // Sleep for 1 millisecond
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

    // read end of the pipe to non-blocking mode
    fcntl(pipeParentToChild.fds[0], F_SETFL, O_NONBLOCK);

    while(true)
    {
        // Check for phase change message from the parent
        int bytesRead =
            read(pipeParentToChild.fds[0], buffer, sizeof(buffer) - 1);
        if(bytesRead > 0)
        {
            buffer[bytesRead] = '\0'; // Ensure null termination

            if(verbose)
            {
                std::cout << "Child " << childIndex
                          << ": Received phase message: " << buffer << "\n";
            }

            if(strcmp(buffer, "RED_PED") == 0)
            {
                pedestrianWatcher->processFrame();
                float density = pedestrianWatcher->getInstanceCount();
                sendDensityToParent(density);
            }
            else if(strcmp(buffer, "GREEN_PED") == 0)
            {
                pedestrianWatcher->processFrame();
                float density = pedestrianWatcher->getInstanceCount();
                sendDensityToParent(density);
            }
            else
            {
                std::cerr << "Child " << childIndex
                          << ": Unknown message received: " << buffer << "\n";
                break; // Exit if an unknown message is received
            }
        }

        // Sleep for a short period to save CPU resource
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

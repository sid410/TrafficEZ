#include "ChildProcess.h"
#include "WatcherSpawner.h"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <unistd.h>

ChildProcess::ChildProcess(int childIndex,
                           Pipe& pipeParentToChild,
                           Pipe& pipeChildToParent)
    : childIndex(childIndex)
    , pipeParentToChild(pipeParentToChild)
    , pipeChildToParent(pipeChildToParent)
{}

void ChildProcess::runVehicle(bool debug, int vehicleId)
{
    closeUnusedPipes();

    WatcherSpawner spawner;
    Watcher* vehicleWatcher;

    std::ostringstream configStream;
    configStream << "vehicle" << vehicleId << ".yaml";
    std::string configFile = configStream.str();

    if(debug)
    {
        vehicleWatcher = spawner.spawnWatcher(
            WatcherType::VEHICLE, RenderMode::GUI, "debug.mp4", configFile);
    }
    else
    {
        vehicleWatcher = spawner.spawnWatcher(WatcherType::VEHICLE,
                                              RenderMode::HEADLESS,
                                              "debug.mp4",
                                              configFile);
    }

    char buffer[128];
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
            std::cout << "Child " << childIndex
                      << ": Received phase message: " << buffer << "\n";

            if(strcmp(buffer, "RED_PHASE") == 0)
            {
                // Send traffic density back to the parent
                float density = vehicleWatcher->getTrafficDensity();
                snprintf(buffer, sizeof(buffer), "%.2f", density);
                if(write(pipeChildToParent.fds[1],
                         buffer,
                         strlen(buffer) + 1) == -1)
                {
                    std::cerr
                        << "Child " << childIndex
                        << ": Failed to write to pipe: " << strerror(errno)
                        << "\n";
                    break;
                }

                isStateGreen = false;
                vehicleWatcher->setCurrentTrafficState(TrafficState::RED_PHASE);
            }
            else if(strcmp(buffer, "GREEN_PHASE") == 0)
            {
                // Process 1 red frame first before changing
                vehicleWatcher->processFrame();

                // Send traffic density back to the parent
                float density = vehicleWatcher->getTrafficDensity();
                snprintf(buffer, sizeof(buffer), "%.2f", density);
                if(write(pipeChildToParent.fds[1],
                         buffer,
                         strlen(buffer) + 1) == -1)
                {
                    std::cerr
                        << "Child " << childIndex
                        << ": Failed to write to pipe: " << strerror(errno)
                        << "\n";
                    break;
                }

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
            usleep(1000); // Sleep for 1 millisecond
        }
    }

    delete vehicleWatcher;
}

void ChildProcess::closeUnusedPipes()
{
    close(pipeParentToChild.fds[1]);
    close(pipeChildToParent.fds[0]);
}

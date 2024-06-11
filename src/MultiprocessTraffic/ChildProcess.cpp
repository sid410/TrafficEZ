#include "ChildProcess.h"
#include "WatcherSpawner.h"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

ChildProcess::ChildProcess(int childIndex,
                           Pipe& pipeParentToChild,
                           Pipe& pipeChildToParent)
    : childIndex(childIndex)
    , pipeParentToChild(pipeParentToChild)
    , pipeChildToParent(pipeChildToParent)
{}

void ChildProcess::run()
{
    closeUnusedPipes();

    WatcherSpawner spawner;
    Watcher* vehicleWatcherGui = spawner.spawnWatcher(
        WatcherType::VEHICLE, RenderMode::GUI, "debug.mp4", "debug_calib.yaml");

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
                float density = vehicleWatcherGui->getTrafficDensity();
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
                vehicleWatcherGui->setCurrentTrafficState(
                    TrafficState::RED_PHASE);
            }
            else if(strcmp(buffer, "GREEN_PHASE") == 0)
            {
                // Process 1 red frame first before changing
                vehicleWatcherGui->processFrame();

                // Send traffic density back to the parent
                float density = vehicleWatcherGui->getTrafficDensity();
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
                vehicleWatcherGui->setCurrentTrafficState(
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
            vehicleWatcherGui->processFrame();
        }
        else
        {
            // Sleep for a short period to save CPU resource
            usleep(10000); // Sleep for 10 milliseconds
        }
    }

    delete vehicleWatcherGui;
}

void ChildProcess::closeUnusedPipes()
{
    close(pipeParentToChild.fds[1]);
    close(pipeChildToParent.fds[0]);
}

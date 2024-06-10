#include "MultiprocessTraffic.h"
#include "WatcherSpawner.h"

MultiprocessTraffic::MultiprocessTraffic(int numChildren)
    : numChildren(numChildren)
{}

void MultiprocessTraffic::start()
{
    createPipes();
    forkChildren();
    parentProcess();
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
    for(int i = 0; i < numChildren; ++i)
    {
        pid_t pid = fork();
        if(pid < 0)
        {
            std::cerr << "Fork failed: " << strerror(errno) << "\n";
            exit(EXIT_FAILURE);
        }
        else if(pid == 0)
        {
            closeUnusedPipesInChild(i);
            childProcess(i);
            close(pipesParentToChild[i].fds[0]);
            close(pipesChildToParent[i].fds[1]);
            exit(EXIT_SUCCESS);
        }
        else
        {
            childPids.push_back(pid);
        }
    }
}

void MultiprocessTraffic::closeUnusedPipesInParent()
{
    for(int i = 0; i < numChildren; ++i)
    {
        close(pipesParentToChild[i].fds[0]);
        close(pipesChildToParent[i].fds[1]);
    }
}

void MultiprocessTraffic::closeUnusedPipesInChild(int childIndex)
{
    for(int i = 0; i < numChildren; ++i)
    {
        if(i != childIndex)
        {
            close(pipesParentToChild[i].fds[0]);
            close(pipesParentToChild[i].fds[1]);
            close(pipesChildToParent[i].fds[0]);
            close(pipesChildToParent[i].fds[1]);
        }
    }
    close(pipesParentToChild[childIndex].fds[1]);
    close(pipesChildToParent[childIndex].fds[0]);
}

void MultiprocessTraffic::parentProcess()
{
    closeUnusedPipesInParent();

    char buffer[128];

    const char* redPhaseMsg = "RED_PHASE";
    const char* greenPhaseMsg = "GREEN_PHASE";

    // Traffic state phases
    const int redPhaseDuration = 1000;
    const int greenPhaseDuration = 500;

    // Initial state
    const char* currentPhaseMsg = greenPhaseMsg;

    while(true)
    {
        for(int i = 0; i < numChildren; ++i)
        {
            std::cout << "Parent: Sending phase message to child " << i << ": "
                      << currentPhaseMsg << "\n";
            if(write(pipesParentToChild[i].fds[1],
                     currentPhaseMsg,
                     strlen(currentPhaseMsg) + 1) == -1)
            {
                std::cerr << "Parent: Failed to write to pipe: "
                          << strerror(errno) << "\n";
                break;
            }
        }

        for(int i = 0; i < numChildren; ++i)
        {
            int bytesRead =
                read(pipesChildToParent[i].fds[0], buffer, sizeof(buffer) - 1);
            if(bytesRead <= 0)
            {
                std::cerr << "Parent: Failed to read from pipe or EOF reached: "
                          << strerror(errno) << "\n";
                break;
            }
            buffer[bytesRead] = '\0'; // Ensure null termination

            std::cout << "Traffic Density from child " << i << ": " << buffer
                      << "\n";
        }

        // Simulate time passing
        usleep(currentPhaseMsg == redPhaseMsg ? redPhaseDuration * 10000
                                              : greenPhaseDuration * 10000);

        // Toggle phase
        currentPhaseMsg =
            (currentPhaseMsg == redPhaseMsg) ? greenPhaseMsg : redPhaseMsg;
    }

    for(auto pid : childPids)
    {
        waitpid(pid, nullptr, 0);
    }
}

void MultiprocessTraffic::childProcess(int childIndex)
{
    WatcherSpawner spawner;
    Watcher* vehicleWatcherGui = spawner.spawnWatcher(
        WatcherType::VEHICLE, RenderMode::GUI, "debug.mp4", "debug_calib.yaml");

    char buffer[128];
    bool isStateGreen = false;

    // read end of the pipe to non-blocking mode
    fcntl(pipesParentToChild[childIndex].fds[0], F_SETFL, O_NONBLOCK);

    while(true)
    {
        // Check for phase change message from the parent
        int bytesRead = read(
            pipesParentToChild[childIndex].fds[0], buffer, sizeof(buffer) - 1);
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
                if(write(pipesChildToParent[childIndex].fds[1],
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
                if(write(pipesChildToParent[childIndex].fds[1],
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

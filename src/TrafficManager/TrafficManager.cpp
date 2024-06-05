#include "TrafficManager.h"
#include "WatcherSpawner.h"

TrafficManager::TrafficManager(int numCars,
                               int numPedestrians,
                               bool debug,
                               bool calib)
    : numberOfCars(numCars)
    , numberOfPedestrians(numPedestrians)
    , debugMode(debug)
    , calibMode(calib)
{}

void TrafficManager::start()
{
    std::cout << "TrafficManager starting...\n";
    std::cout << "Number of Cars: " << numberOfCars << "\n";
    std::cout << "Number of Pedestrians: " << numberOfPedestrians << "\n";
    std::cout << "Debug Mode: " << (debugMode ? "true" : "false") << "\n";

    if(calibMode)
    {
        handleCalibrationMode();
    }

    if(debugMode)
    {
        handleDebugMode();
    }

    std::cout << "TrafficManager ended.\n";
}

void TrafficManager::handleCalibrationMode()
{
    WatcherSpawner spawner;

    Watcher* calibrateWatcherGui = spawner.spawnWatcher(WatcherType::CALIBRATE,
                                                        RenderMode::GUI,
                                                        "debug.mp4",
                                                        "debug_calib.yaml");
    delete calibrateWatcherGui;
}

void TrafficManager::handleDebugMode()
{
    int pipe_parent_to_child[2];
    int pipe_child_to_parent[2];
    if(pipe(pipe_parent_to_child) == -1 || pipe(pipe_child_to_parent) == -1)
    {
        std::cerr << "Pipe creation failed: " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if(pid < 0)
    {
        std::cerr << "Fork failed: " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }
    else if(pid == 0)
    {
        // Child process
        close(pipe_parent_to_child[1]); // Close unused write end
        close(pipe_child_to_parent[0]); // Close unused read end
        childProcess(pipe_parent_to_child, pipe_child_to_parent);
        close(
            pipe_parent_to_child[0]); // Close read end after child process ends
        close(pipe_child_to_parent
                  [1]); // Close write end after child process ends
        exit(EXIT_SUCCESS);
    }
    else
    {
        // Parent process
        close(pipe_parent_to_child[0]); // Close unused read end
        close(pipe_child_to_parent[1]); // Close unused write end
        parentProcess(pipe_parent_to_child, pipe_child_to_parent);
        close(pipe_parent_to_child
                  [1]); // Close write end after parent process ends
        close(pipe_child_to_parent
                  [0]); // Close read end after parent process ends
        wait(nullptr); // Wait for the child process to finish
    }
}

void TrafficManager::parentProcess(int pipe_parent_to_child[2],
                                   int pipe_child_to_parent[2])
{
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
        std::cout << "Parent: Sending phase message: " << currentPhaseMsg
                  << "\n";
        if(write(pipe_parent_to_child[1],
                 currentPhaseMsg,
                 strlen(currentPhaseMsg) + 1) == -1)
        {
            std::cerr << "Parent: Failed to write to pipe: " << strerror(errno)
                      << "\n";
            break;
        }

        // Block and wait for traffic density from the child
        int bytesRead =
            read(pipe_child_to_parent[0], buffer, sizeof(buffer) - 1);
        if(bytesRead <= 0)
        {
            std::cerr << "Parent: Failed to read from pipe or EOF reached: "
                      << strerror(errno) << "\n";
            break;
        }
        buffer[bytesRead] = '\0'; // Ensure null termination

        std::cout << (currentPhaseMsg == redPhaseMsg
                          ? "Traffic Density Previous Green: "
                          : "Traffic Density Previous Red: ")
                  << buffer << "\n";

        // Simulate time passing
        usleep(currentPhaseMsg == redPhaseMsg ? redPhaseDuration * 10000
                                              : greenPhaseDuration * 10000);

        // Toggle phase
        currentPhaseMsg =
            (currentPhaseMsg == redPhaseMsg) ? greenPhaseMsg : redPhaseMsg;
    }
}

void TrafficManager::childProcess(int pipe_parent_to_child[2],
                                  int pipe_child_to_parent[2])
{
    WatcherSpawner spawner;
    Watcher* vehicleWatcherGui = spawner.spawnWatcher(
        WatcherType::VEHICLE, RenderMode::GUI, "debug.mp4", "debug_calib.yaml");

    char buffer[128];
    bool isStateGreen = false;

    // read end of the pipe to non-blocking mode
    fcntl(pipe_parent_to_child[0], F_SETFL, O_NONBLOCK);

    while(true)
    {
        // Check for phase change message from the parent
        int bytesRead =
            read(pipe_parent_to_child[0], buffer, sizeof(buffer) - 1);
        if(bytesRead > 0)
        {
            buffer[bytesRead] = '\0'; // Ensure null termination
            std::cout << "Child: Received phase message: " << buffer << "\n";

            if(strcmp(buffer, "RED_PHASE") == 0)
            {
                // Send traffic density back to the parent
                float density = vehicleWatcherGui->getTrafficDensity();
                snprintf(buffer, sizeof(buffer), "%.2f", density);
                if(write(pipe_child_to_parent[1], buffer, strlen(buffer) + 1) ==
                   -1)
                {
                    std::cerr
                        << "Child: Failed to write to pipe: " << strerror(errno)
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
                if(write(pipe_child_to_parent[1], buffer, strlen(buffer) + 1) ==
                   -1)
                {
                    std::cerr
                        << "Child: Failed to write to pipe: " << strerror(errno)
                        << "\n";
                    break;
                }

                isStateGreen = true;
                vehicleWatcherGui->setCurrentTrafficState(
                    TrafficState::GREEN_PHASE);
            }
            else
            {
                std::cerr << "Child: Unknown message received: " << buffer
                          << "\n";
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
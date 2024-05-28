#include "TrafficManager.h"
#include "WatcherSpawner.h"
#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

TrafficManager::TrafficManager(int numCars,
                               int numPedestrians,
                               bool debug,
                               bool calib)
    : numberOfCars(numCars)
    , numberOfPedestrians(numPedestrians)
    , debugMode(debug)
    , calibMode(calib)
{
    if(debugMode)
    {
        if(pipe(pipe1) == -1 || pipe(pipe2) == -1)
        {
            std::cerr << "Failed to create pipes.\n";
            exit(EXIT_FAILURE);
        }
    }
}

void TrafficManager::start()
{
    std::cout << "TrafficManager starting...\n";
    std::cout << "Number of Cars: " << numberOfCars << "\n";
    std::cout << "Number of Pedestrians: " << numberOfPedestrians << "\n";
    std::cout << "Debug Mode: " << (debugMode ? "true" : "false") << "\n";

    WatcherSpawner spawner;

    if(calibMode)
    {
        Watcher* calibrateWatcherGui =
            spawner.spawnWatcher(WatcherType::CALIBRATE,
                                 RenderMode::GUI,
                                 "debug.mp4",
                                 "debug_calib.yaml");
        delete calibrateWatcherGui;
    }
    if(debugMode)
    {
        forkChildProcesses();
    }

    std::cout << "TrafficManager ended.\n";
}

void TrafficManager::forkChildProcesses()
{
    pid_t pid1 = fork();
    if(pid1 == -1)
    {
        std::cerr << "Failed to fork first child.\n";
        exit(EXIT_FAILURE);
    }
    else if(pid1 == 0)
    {
        // First child process
        close(pipe1[1]); // Close write end of pipe1
        handleChildProcess(pipe1[0], pipe1[1]);
        exit(EXIT_SUCCESS);
    }
    else
    {
        childPids.push_back(pid1);
    }

    pid_t pid2 = fork();
    if(pid2 == -1)
    {
        std::cerr << "Failed to fork second child.\n";
        exit(EXIT_FAILURE);
    }
    else if(pid2 == 0)
    {
        // Second child process
        close(pipe2[1]); // Close write end of pipe2
        handleChildProcess(pipe2[0], pipe2[1]);
        exit(EXIT_SUCCESS);
    }
    else
    {
        childPids.push_back(pid2);
    }

    // Parent process
    close(pipe1[0]); // Close read end of pipe1
    close(pipe2[0]); // Close read end of pipe2

    handleCommunication(pipe1[0]);
    handleCommunication(pipe2[0]);

    // Wait for child processes to finish
    for(int pid : childPids)
    {
        waitpid(pid, nullptr, 0);
    }
}

void TrafficManager::handleCommunication(int readPipe)
{
    // Read response from child
    char buffer[256];
    while(read(readPipe, buffer, sizeof(buffer)) > 0)
    {
        std::cout << "Received from child: " << buffer << "\n";
    }
}

void TrafficManager::handleChildProcess(int readPipe, int writePipe)
{
    WatcherSpawner spawner;
    Watcher* vehicleWatcherGui = spawner.spawnWatcher(
        WatcherType::VEHICLE, RenderMode::GUI, "debug.mp4", "debug_calib.yaml");

    std::string message;
    write(writePipe, message.c_str(), message.size() + 1);

    delete vehicleWatcherGui;
}
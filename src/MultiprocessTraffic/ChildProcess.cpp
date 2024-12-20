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
                      << ": Received phase message: " << buffer << "\n"
                      << std::flush;
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
        std::unordered_map<std::string, int> vehicles =
            watcher->getVehicleTypeAndCount();

        float speed = watcher->getAverageSpeed();

        // send the previous green vehicle density
        float density = watcher->getTrafficDensity();

        sendPhaseMessageToParent(density, speed, vehicles);

        isStateGreen = false;
        watcher->setCurrentTrafficState(TrafficState::RED_PHASE);
        break;
    }

    case GREEN_PHASE: {
        std::unordered_map<std::string, int> vehicles =
            watcher->getVehicleTypeAndCount();

        float speed = watcher->getAverageSpeed();

        // send the previous red vehicle density
        watcher->processFrame();
        float density = watcher->getTrafficDensity();

        sendPhaseMessageToParent(density, speed, vehicles);

        isStateGreen = true;
        watcher->setCurrentTrafficState(TrafficState::GREEN_PHASE);
        break;
    }

    case RED_PED: {
        // send the waiting pedestrian count
        watcher->processFrame();
        float density = watcher->getInstanceCount();
        sendPhaseMessageToParent(density, 0.0f);

        break;
    }

    case GREEN_PED: {
        // ignore the already walking pedestrian
        sendPhaseMessageToParent(0.0f, 0.0f);

        break;
    }

    case UNKNOWN:
    default: {
        std::cerr << "Child " << childIndex << ": Unknown message received.\n";
        break;
    }
    }
}

void ChildProcess::sendPhaseMessageToParent(
    float density, float speed, std::unordered_map<std::string, int> vehicles)
{
    char buffer[BUFFER_SIZE] = {0};
    int offset = 0;

    // Safely format and append density
    int written =
        snprintf(buffer + offset, BUFFER_SIZE - offset, "%.2f;", density);
    if(written < 0 || written >= BUFFER_SIZE - offset)
    {
        std::cerr << "Child " << childIndex
                  << ": Buffer overflow while writing density.\n";
        return;
    }
    offset += written;

    // Safely format and append speed
    written = snprintf(buffer + offset, BUFFER_SIZE - offset, "%.2f;", speed);
    if(written < 0 || written >= BUFFER_SIZE - offset)
    {
        std::cerr << "Child " << childIndex
                  << ": Buffer overflow while writing speed.\n";
        return;
    }
    offset += written;

    // Convert vehicle counts to string
    std::ostringstream oss;
    for(const auto& entry : vehicles)
    {
        oss << entry.first << ":" << entry.second << ",";
    }

    std::string vehicleData = oss.str();
    if(!vehicleData.empty() && vehicleData.back() == ',')
    {
        vehicleData.pop_back(); // Remove trailing comma
    }

    // Safely append vehicle data
    written = snprintf(
        buffer + offset, BUFFER_SIZE - offset, "%s", vehicleData.c_str());
    if(written < 0 || written >= BUFFER_SIZE - offset)
    {
        std::cerr << "Child " << childIndex
                  << ": Buffer overflow while writing vehicle data.\n";
        return;
    }

    // Ensure buffer is null-terminated
    buffer[BUFFER_SIZE - 1] = '\0';

    // Set pipe to non-blocking mode
    fcntl(pipeChildToParent.fds[1], F_SETFL, O_NONBLOCK);

    // Retry writing to the pipe with a maximum retry count
    int retryCount = 5;
    ssize_t bytesWritten = 0;
    while(retryCount-- > 0)
    {
        bytesWritten =
            write(pipeChildToParent.fds[1], buffer, strlen(buffer) + 1);

        if(bytesWritten == -1)
        {
            if(errno == EAGAIN)
            {
                std::cerr << "Child " << childIndex
                          << ": Pipe is full, retrying...\n";
                usleep(1000); // Sleep for 1 millisecond before retrying
            }
            else
            {
                std::cerr << "Child " << childIndex
                          << ": Failed to write data to pipe: "
                          << strerror(errno) << "\n";
                return;
            }
        }
        else
        {
            // Successfully written
            break;
        }
    }

    if(bytesWritten == -1)
    {
        std::cerr << "Child " << childIndex
                  << ": Failed to write after retries.\n";
    }
}

void ChildProcess::closeUnusedPipes()
{
    close(pipeParentToChild.fds[1]);
    close(pipeChildToParent.fds[0]);
}

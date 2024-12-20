#include "TelnetRelayController.h"
#include <bits/this_thread_sleep.h>
#include <sstream>
#include <sys/select.h>
#include <unistd.h>
#include <unordered_map>

#define PORT 23

bool waitForData(int sock, int timeoutSeconds)
{
    fd_set readfds;
    struct timeval timeout;

    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);

    timeout.tv_sec = timeoutSeconds;
    timeout.tv_usec = 0;

    int result = select(sock + 1, &readfds, NULL, NULL, &timeout);
    if(result > 0 && FD_ISSET(sock, &readfds))
    {
        return true;
    }
    return false;
}

TelnetRelayController::TelnetRelayController(
    const std::string& ip,
    const std::string& user,
    const std::string& pass,
    const std::vector<std::vector<PhaseMessageType>>& phaseData,
    bool verboseMode)
    : relayIP(ip)
    , username(user)
    , password(pass)
    , phases(phaseData)
    , verbose(verboseMode)
    , sock(-1)
{

    if(!connectToRelay())
    {
        std::cerr << "Error: Failed to connect to relay module!" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(!authenticate())
    {
        std::cerr << "Error: Failed to authenticate with relay module!"
                  << std::endl;
        exit(EXIT_FAILURE);
    }
}

TelnetRelayController::~TelnetRelayController()
{
    if(sock >= 0)
    {
        close(sock);
    }
}

bool TelnetRelayController::connectToRelay()
{
    struct sockaddr_in server_addr;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        std::cerr << "Socket creation failed" << std::endl;
        return false;
    }

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(relayIP.c_str());

    if(connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
    {
        close(sock);
        return false;
    }

    std::cout << "Connected to relay at " << relayIP << std::endl;
    return true;
}

bool TelnetRelayController::authenticate()
{
    sendCommand(username);
    sendCommand("��-" + password);
    std::string response = receiveResponse();

    return (response.find(">") != std::string::npos ||
            response.find("Logged in successfully") != std::string::npos);
}

bool TelnetRelayController::reconnect()
{
    close(sock);
    if(!connectToRelay())
    {
        return false;
    };
    if(!authenticate())
    {
        return false;
    };
    return true;
}

void TelnetRelayController::sendCommand(const std::string& command)
{
    std::string fullCommand = command + "\r\n";

    ssize_t bytesSent =
        send(sock, fullCommand.c_str(), fullCommand.length(), 0);
    if(bytesSent < 0)
    {
        std::cerr << "Failed to send command, attempting to reconnect..."
                  << std::endl;
        if(reconnect())
        {
            sendCommand(command);
        }
        else
        {
            std::cerr << "Reconnection failed." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

std::string TelnetRelayController::receiveResponse(int retries,
                                                   int timeoutSeconds)
{
    std::string fullResponse;
    char buffer[1024];

    for(int i = 0; i < retries; ++i)
    {
        if(waitForData(sock, timeoutSeconds))
        {
            ssize_t bytesRead = read(sock, buffer, sizeof(buffer) - 1);
            if(bytesRead > 0)
            {
                buffer[bytesRead] = '\0'; // Null-terminate the buffer
                fullResponse += buffer; // Append to the full response
            }
        }
        else
        {
            std::cerr << "Timeout waiting for data..." << std::endl;
        }
    }

    if(verbose)
    {
        std::cout << "Full response received: " << fullResponse << std::endl;
    }
    return fullResponse;
}

void TelnetRelayController::turnOnAllRelay(std::vector<int> relayNumbers)
{
    std::string hex = getHexCommand(relayNumbers);
    if(verbose)
    {
        std::cout << "Sent Command: " << hex << std::endl;
    }
    sendCommand("relay writeall " + hex);
}

void TelnetRelayController::turnOffAllRelay()
{
    sendCommand("reset");
}

std::string TelnetRelayController::getRelayStatus()
{
    sendCommand("relay readall");
    return receiveResponse();
}

std::vector<int>
TelnetRelayController::getOnChannelsFromStatus(const std::string& relayStatus)
{
    unsigned int binaryValue = std::stoul(relayStatus, nullptr, 16);
    std::vector<int> onChannels;

    for(int channel = 0; channel < 16; ++channel)
    {
        if(binaryValue & (1 << channel))
        {
            onChannels.push_back(channel);
        }
    }

    return onChannels;
}

std::string
TelnetRelayController::getHexCommand(const std::vector<int>& onChannels)
{
    unsigned int binaryValue = 0;

    for(int channel : onChannels)
    {
        binaryValue |= (1 << channel);
    }

    std::stringstream hexStream;
    hexStream << std::hex << binaryValue; // hex should be lowercase

    // Ensure the hex command is 4 characters long
    std::string hexCommand = hexStream.str();
    return std::string(4 - hexCommand.length(), '0') + hexCommand;
}

void TelnetRelayController::setPhaseCycle(int cycle)
{
    if(cycle < 0 || cycle >= phases.size())
    {
        std::cerr << "Invalid phase cycle: " << cycle << std::endl;
        return;
    }
    currentCycle = cycle;
}

void TelnetRelayController::executePhase()
{
    if(currentCycle < 0 || currentCycle >= phases.size())
    {
        std::cerr << "Invalid current cycle: " << currentCycle << std::endl;
        return;
    }

    std::vector<PhaseMessageType> phase = phases[currentCycle];
    int relayIndex = 0;
    std::vector<int> onChannels = {};

    for(const PhaseMessageType& p : phase)
    {

        switch(p)
        {
        case GREEN_PHASE:
            relayIndex++;
            onChannels.push_back(relayIndex++);
            relayIndex++;
            break;
        case RED_PHASE:
            onChannels.push_back(relayIndex++);
            relayIndex++;
            relayIndex++;
            break;
        case GREEN_PED:
            relayIndex++;
            onChannels.push_back(relayIndex++);
            break;
        case RED_PED:
            onChannels.push_back(relayIndex++);
            relayIndex++;
            break;
        default:
            break;
        }
    }

    turnOnAllRelay(onChannels);
}

void TelnetRelayController::executeTransitionPhase()
{
    if(currentCycle < 0 || currentCycle >= phases.size())
    {
        std::cerr << "Invalid current cycle: " << currentCycle << std::endl;
        return;
    }

    int nextPhaseIndex = (currentCycle + 1) % phases.size();
    std::vector<PhaseMessageType> transitionPhase =
        deriveTransitionPhase(phases[currentCycle], phases[nextPhaseIndex]);

    int relayIndex = 0;
    std::vector<int> onChannels = {};

    for(const PhaseMessageType& p : transitionPhase)
    {

        switch(p)
        {
        case GREEN_PHASE:
            relayIndex++;
            onChannels.push_back(relayIndex++);
            relayIndex++;
            break;
        case RED_PHASE:
            onChannels.push_back(relayIndex++);
            relayIndex++;
            relayIndex++;
            break;
        case YELLOW_PHASE:
            relayIndex++;
            relayIndex++;
            onChannels.push_back(relayIndex++);
            break;
        case GREEN_PED:
            relayIndex++;
            onChannels.push_back(relayIndex++);
            break;
        case RED_PED:
            onChannels.push_back(relayIndex++);
            relayIndex++;
            break;
        default:
            break;
        }
    }

    turnOnAllRelay(onChannels);
}

std::vector<PhaseMessageType> TelnetRelayController::deriveTransitionPhase(
    const std::vector<PhaseMessageType>& currentPhase,
    const std::vector<PhaseMessageType>& nextPhase)
{
    std::vector<PhaseMessageType> transitionPhase;

    for(size_t i = 0; i < currentPhase.size(); ++i)
    {
        if(currentPhase[i] == GREEN_PHASE && nextPhase[i] == RED_PHASE)
        {
            transitionPhase.push_back(YELLOW_PHASE);
        }
        else if(currentPhase[i] == RED_PHASE && nextPhase[i] == GREEN_PHASE)
        {
            transitionPhase.push_back(RED_PHASE);
        }
        else if(currentPhase[i] == GREEN_PED && nextPhase[i] == RED_PED)
        {
            transitionPhase.push_back(RED_PED);
        }
        else
        {
            transitionPhase.push_back(currentPhase[i]);
        }
    }

    return transitionPhase;
}

bool TelnetRelayController::setStandbyMode(
    const std::vector<int>& yellowChannels, int durationMs, int flashIntervalMs)
{
    if(verbose)
    {
        std::cout << "Switching to standby mode...\n";
    }

    if(durationMs < -1 || flashIntervalMs <= 0)
    {
        std::cerr << "Invalid parameters: durationMs=" << durationMs
                  << ", flashIntervalMs=" << flashIntervalMs << '\n';
        return false;
    }

    if(yellowChannels.empty())
    {
        std::cerr << "Yellow channels list is empty. Unable to proceed.\n";
        return false;
    }

    auto startTime = std::chrono::steady_clock::now();

    while(true)
    {
        turnOnAllRelay(yellowChannels);
        std::this_thread::sleep_for(std::chrono::milliseconds(flashIntervalMs));

        turnOffAllRelay();
        std::this_thread::sleep_for(std::chrono::milliseconds(flashIntervalMs));

        if(durationMs != -1)
        {
            auto elapsedTime = std::chrono::steady_clock::now() - startTime;
            if(std::chrono::duration_cast<std::chrono::milliseconds>(
                   elapsedTime)
                   .count() >= durationMs)
            {
                turnOffAllRelay(); // Ensure lights are off
                break;
            }
        }
    }

    return true;
}

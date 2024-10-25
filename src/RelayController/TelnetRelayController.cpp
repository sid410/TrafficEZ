#include "TelnetRelayController.h"
#include <bits/this_thread_sleep.h>
#include <sstream>
#include <sys/select.h>
#include <unistd.h>
#include <unordered_map>

#define PORT 23

const std::unordered_map<PhaseMessageType, std::vector<int>> channelMap = {
    {PhaseMessageType::RED_PHASE, {0, 3, 6, 9}},
    {PhaseMessageType::GREEN_PHASE, {1, 4, 7, 10}},
    {PhaseMessageType::YELLOW_PHASE, {2, 5, 8, 11}},
    {PhaseMessageType::RED_PED, {12, 14}},
    {PhaseMessageType::GREEN_PED, {13, 15}}};

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
        std::cerr << "Failed to connect to relay module!" << std::endl;
    }

    if(!authenticate())
    {
        std::cerr << "Failed to authenticate with relay module!" << std::endl;
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
        perror("Socket creation failed");
        return false;
    }

    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(relayIP.c_str());

    if(connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection to relay failed");
        close(sock);
        return false;
    }

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
    return connectToRelay();
}

void TelnetRelayController::sendCommand(const std::string& command)
{
    std::string fullCommand = command + "\r\n";

    ssize_t bytesSent =
        send(sock, fullCommand.c_str(), fullCommand.length(), 0);
    if(bytesSent < 0)
    {
        perror("Failed to send command, attempting to reconnect...");
        if(reconnect())
        {
            sendCommand(command);
        }
        else
        {
            std::cerr << "Reconnection failed." << std::endl;
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

    std::cout << "Full response received: " << fullResponse << std::endl;
    return fullResponse;
}

void TelnetRelayController::turnOnRelay(int relayNumber)
{
    if(relayNumber < 0 || relayNumber > 15)
    {
        std::cerr << "Invalid relay number: " << relayNumber << std::endl;
        return;
    }

    sendCommand("relay on " + std::to_string(relayNumber));
}

void TelnetRelayController::turnOffRelay(int relayNumber)
{
    if(relayNumber < 0 || relayNumber > 15)
    {
        std::cerr << "Invalid relay number: " << relayNumber << std::endl;
        return;
    }

    sendCommand("relay off " + std::to_string(relayNumber));
}

void TelnetRelayController::turnOnAllRelay(std::vector<int> relayNumbers)
{
    std::string hex = getHexCommand(relayNumbers);
    std::cout << "Sent Command: " << hex << std::endl;
    TelnetRelayController::sendCommand("relay writeall " + hex);
}

std::string TelnetRelayController::getRelayStatus()
{
    sendCommand("relay readall");
    return receiveResponse();
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

bool TelnetRelayController::standbyMode()
{
    std::string hex = getHexCommand({2, 5, 8, 11});
    while(true)
    {
        sendCommand("relay writeall " + hex);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        sendCommand("reset");
    }
    exit(EXIT_SUCCESS);
}

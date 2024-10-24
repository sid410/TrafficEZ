#include "TelnetRelayController.h"
#include <sys/select.h>

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

TelnetRelayController::TelnetRelayController(const std::string& ip,
                                             const std::string& user,
                                             const std::string& pass)
    : relayIP(ip)
    , username(user)
    , password(pass)
    , sock(-1)
{

    if(!connectToRelay())
    {
        std::cerr << "Failed to connect to relay module!" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(!authenticate())
    {
        std::cerr << "Failed to authenticate with relay module!" << std::endl;
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
    if(relayNumber < 1 || relayNumber > 16)
    {
        std::cerr << "Invalid relay number: " << relayNumber << std::endl;
        return;
    }

    sendCommand("relay on " + std::to_string(relayNumber));
}

void TelnetRelayController::turnOffRelay(int relayNumber)
{
    if(relayNumber < 1 || relayNumber > 16)
    {
        std::cerr << "Invalid relay number: " << relayNumber << std::endl;
        return;
    }

    sendCommand("relay off " + std::to_string(relayNumber));
}

std::string TelnetRelayController::getRelayStatus()
{
    sendCommand("relay readall");
    return receiveResponse();
}

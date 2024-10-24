#ifndef TELNET_RELAY_CONTROLLER_H
#define TELNET_RELAY_CONTROLLER_H

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <stdexcept>
#include <string>
#include <unistd.h>

#define PORT 23

class TelnetRelayController
{
public:
    explicit TelnetRelayController(const std::string& ip = "192.168.1.5",
                                   const std::string& user = "ezadmin",
                                   const std::string& pass = "ez@dmin");
    ~TelnetRelayController();

    void sendCommand(const std::string& command);
    std::string receiveResponse(int retries = 5, int timeoutSeconds = 5);
    void turnOnRelay(int relayNumber);
    void turnOffRelay(int relayNumber);
    std::string getRelayStatus();

protected:
    bool connectToRelay();
    bool authenticate();
    bool reconnect();

private:
    std::string relayIP;
    std::string username;
    std::string password;
    int sock;
};

#endif

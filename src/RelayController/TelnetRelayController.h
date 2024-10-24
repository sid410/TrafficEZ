#ifndef TELNET_RELAY_CONTROLLER_H
#define TELNET_RELAY_CONTROLLER_H

#include "PhaseMessageType.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>

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

    std::string getHexCommand(const std::vector<int>& onChannels);

    void executePhase();

    void executeTransitionPhase();

    std::vector<PhaseMessageType>
    deriveTransitionPhase(const std::vector<PhaseMessageType>& currentPhase,
                          const std::vector<PhaseMessageType>& nextPhase);

protected:
    bool connectToRelay();
    bool authenticate();
    bool reconnect();

private:
    std::string relayIP;
    std::string username;
    std::string password;
    int sock;

    int currentCycle;
    std::vector<std::vector<PhaseMessageType>> phases;
};

#endif

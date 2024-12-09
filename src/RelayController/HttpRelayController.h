#ifndef RELAY_CONTROLLER_H
#define RELAY_CONTROLLER_H

#include "PhaseMessageType.h"
#include <curl/curl.h>
#include <vector>

/* This is an implementation for the initial relay controller used, the 
NC100 16-Channel Ethernet Controller Relay Board.
*/
class HttpRelayController
{
public:
    HttpRelayController();
    ~HttpRelayController();

    void initialize(const std::vector<std::vector<PhaseMessageType>>& phaseData,
                    const std::string& baseUrl,
                    bool verboseMode = false);

    void setPhaseCycle(int cycle);
    void executePhase();
    void executeTransitionPhase();

private:
    std::vector<std::vector<PhaseMessageType>> phases;
    std::string baseUrl;
    bool verbose;

    CURL* curl;

    int currentCycle;

    static size_t
    writeCallback(void* contents, size_t size, size_t nmemb, std::string* s);

    void sendRequest(const std::string& url);
    void turnOnRelay(int relayNumber);
    void turnOffRelay(int relayNumber);
    void controlRelay(int relayNumber, bool turnOn);

    std::vector<PhaseMessageType>
    deriveTransitionPhase(const std::vector<PhaseMessageType>& currentPhase,
                          const std::vector<PhaseMessageType>& nextPhase);
};

#endif
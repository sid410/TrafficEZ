#ifndef RELAY_CONTROLLER_H
#define RELAY_CONTROLLER_H

#include "PhaseMessageType.h"
#include <curl/curl.h>
#include <vector>

class RelayController
{
public:
    RelayController();
    ~RelayController();

    void initialize(const std::vector<std::vector<PhaseMessageType>>& phaseData,
                    const std::string& baseUrl,
                    bool verboseMode = false);

    void setPhaseCycle(int cycle);
    void executePhase();
    void executeTransitionPhase();
    void startFlashingYellow(int flashDuration);   
    void stopFlashingYellow();

private:
    bool verbose;
    bool flashing;

    std::vector<int> yellowRelayPins {3, 6, 9, 12};

    CURL* curl;
    std::string baseUrl;

    int currentCycle;
    std::vector<std::vector<PhaseMessageType>> phases;

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
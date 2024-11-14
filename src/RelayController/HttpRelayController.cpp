#include "HttpRelayController.h"
#include <iomanip>
#include <iostream>
#include <sstream>

HttpRelayController::HttpRelayController()
    : currentCycle(0)
    , verbose(false)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
}

HttpRelayController::~HttpRelayController()
{
    if(curl)
    {
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

void HttpRelayController::initialize(
    const std::vector<std::vector<PhaseMessageType>>& phaseData,
    const std::string& baseUrl,
    bool verboseMode)
{
    phases = phaseData;
    this->baseUrl = baseUrl;
    verbose = verboseMode;
}

void HttpRelayController::setPhaseCycle(int cycle)
{
    if(cycle < 0 || cycle >= phases.size())
    {
        std::cerr << "Invalid phase cycle: " << cycle << std::endl;
        return;
    }
    currentCycle = cycle;
}

void HttpRelayController::executePhase()
{
    if(currentCycle < 0 || currentCycle >= phases.size())
    {
        std::cerr << "Invalid current cycle: " << currentCycle << std::endl;
        return;
    }

    std::vector<PhaseMessageType> phase = phases[currentCycle];
    int relayIndex = 1;

    for(const PhaseMessageType& p : phase)
    {
        switch(p)
        {
        case GREEN_PHASE:
            controlRelay(relayIndex++, false);
            controlRelay(relayIndex++, true);
            controlRelay(relayIndex++, false);
            break;
        case RED_PHASE:
            controlRelay(relayIndex++, true);
            controlRelay(relayIndex++, false);
            controlRelay(relayIndex++, false);
            break;
        case GREEN_PED:
            controlRelay(relayIndex++, false);
            controlRelay(relayIndex++, true);
            break;
        case RED_PED:
            controlRelay(relayIndex++, true);
            controlRelay(relayIndex++, false);
            break;
        default:
            break;
        }
    }
}

void HttpRelayController::executeTransitionPhase()
{
    if(currentCycle < 0 || currentCycle >= phases.size())
    {
        std::cerr << "Invalid current cycle: " << currentCycle << std::endl;
        return;
    }

    int nextPhaseIndex = (currentCycle + 1) % phases.size();
    std::vector<PhaseMessageType> transitionPhase =
        deriveTransitionPhase(phases[currentCycle], phases[nextPhaseIndex]);

    int relayIndex = 1;

    for(const PhaseMessageType& p : transitionPhase)
    {
        switch(p)
        {
        case GREEN_PHASE:
            controlRelay(relayIndex++, false);
            controlRelay(relayIndex++, true);
            controlRelay(relayIndex++, false);
            break;
        case RED_PHASE:
            controlRelay(relayIndex++, true);
            controlRelay(relayIndex++, false);
            controlRelay(relayIndex++, false);
            break;
        case YELLOW_PHASE:
            controlRelay(relayIndex++, false);
            controlRelay(relayIndex++, false);
            controlRelay(relayIndex++, true);
            break;
        case GREEN_PED:
            controlRelay(relayIndex++, false);
            controlRelay(relayIndex++, true);
            break;
        case RED_PED:
            controlRelay(relayIndex++, true);
            controlRelay(relayIndex++, false);
            break;
        default:
            break;
        }
    }
}

std::vector<PhaseMessageType> HttpRelayController::deriveTransitionPhase(
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

size_t HttpRelayController::writeCallback(void* contents,
                                          size_t size,
                                          size_t nmemb,
                                          std::string* s)
{
    size_t newLength = size * nmemb;
    try
    {
        s->append((char*) contents, newLength);
    }
    catch(std::bad_alloc& e)
    {
        return 0;
    }
    return newLength;
}

void HttpRelayController::sendRequest(const std::string& url)
{
    if(curl)
    {
        std::string response;

        curl_easy_setopt(curl, CURLOPT_URL, (baseUrl + url).c_str());
        curl_easy_setopt(
            curl, CURLOPT_WRITEFUNCTION, HttpRelayController::writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);

        if(res != CURLE_OK)
        {
            fprintf(stderr,
                    "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        }
        else if(verbose)
        {
            std::cout << "Successfully sent " << baseUrl + url << "\n";
        }
    }
}

void HttpRelayController::turnOnRelay(int relayNumber)
{
    if(relayNumber < 1 || relayNumber > 16)
    {
        std::cerr << "Invalid relay number: " << relayNumber << std::endl;
        return;
    }

    int relayCommand = relayNumber * 2 - 1;

    std::stringstream url;
    url << std::setw(2) << std::setfill('0') << relayCommand;

    sendRequest(url.str());
}

void HttpRelayController::turnOffRelay(int relayNumber)
{
    if(relayNumber < 1 || relayNumber > 16)
    {
        std::cerr << "Invalid relay number: " << relayNumber << std::endl;
        return;
    }

    int relayCommand = (relayNumber - 1) * 2;

    std::stringstream url;
    url << std::setw(2) << std::setfill('0') << relayCommand;

    sendRequest(url.str());
}

void HttpRelayController::controlRelay(int relayNumber, bool turnOn)
{
    if(turnOn)
    {
        turnOnRelay(relayNumber);
    }
    else
    {
        turnOffRelay(relayNumber);
    }
}
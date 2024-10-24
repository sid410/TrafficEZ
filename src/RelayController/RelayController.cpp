#include "RelayController.h"
#include <iomanip>
#include <iostream>
#include <sstream>

RelayController::RelayController()
    : currentCycle(0)
    , verbose(false)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
}

RelayController::~RelayController()
{
    if(curl)
    {
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

void RelayController::initialize(
    const std::vector<std::vector<PhaseMessageType>>& phaseData,
    const std::string& baseUrl,
    bool verboseMode)
{
    phases = phaseData;
    this->baseUrl = baseUrl;
    verbose = verboseMode;
}

void RelayController::setPhaseCycle(int cycle)
{
    if(cycle < 0 || cycle >= phases.size())
    {
        std::cerr << "Invalid phase cycle: " << cycle << std::endl;
        return;
    }
    currentCycle = cycle;
}

const std::unordered_map<PhaseMessageType, std::vector<int>> channelMap = {
    {PhaseMessageType::RED_PHASE, {0, 3, 6, 9}},
    {PhaseMessageType::GREEN_PHASE, {1, 4, 7, 10}},
    {PhaseMessageType::YELLOW_PHASE, {2, 5, 8, 11}},
    {PhaseMessageType::RED_PED, {12, 14}},
    {PhaseMessageType::GREEN_PED, {13, 15}}};

std::string RelayController::getHexCommand(const std::vector<int>& onChannels)
{
    // Step 1: Initialize a variable to hold the binary value
    unsigned int binaryValue = 0;

    // Step 2: Set the bits for each ON channel
    for(int channel : onChannels)
    {
        binaryValue |= (1 << channel);
    }

    // Step 3: Convert the binary value to hexadecimal
    std::stringstream hexStream;
    hexStream << std::uppercase << std::hex << binaryValue;

    // Ensure the hex command is 4 characters long
    std::string hexCommand = hexStream.str();
    return std::string(4 - hexCommand.length(), '0') +
           hexCommand; // Padding with zeros
}

void RelayController::executePhase()
{
    if(currentCycle < 0 || currentCycle >= phases.size())
    {
        std::cerr << "Invalid current cycle: " << currentCycle << std::endl;
        return;
    }

    std::vector<PhaseMessageType> phase = phases[currentCycle];
    int relayIndex = 0;
    // int relayIndex = 1;
    std::vector<int> onChannels = {};

    for(const PhaseMessageType& p : phase)
    {

        switch(p)
        {
        case GREEN_PHASE:
            relayIndex++;
            onChannels.push_back(relayIndex++);
            relayIndex++;
            // controlRelay(relayIndex++, false);
            // controlRelay(relayIndex++, true);
            // controlRelay(relayIndex++, false);
            break;
        case RED_PHASE:
            onChannels.push_back(relayIndex++);
            relayIndex++;
            relayIndex++;
            // controlRelay(relayIndex++, true);
            // controlRelay(relayIndex++, false);
            // controlRelay(relayIndex++, false);
            break;
        case GREEN_PED:
            relayIndex++;
            onChannels.push_back(relayIndex++);
            // controlRelay(relayIndex++, false);
            // controlRelay(relayIndex++, true);
            break;
        case RED_PED:
            onChannels.push_back(relayIndex++);
            relayIndex++;
            // controlRelay(relayIndex++, true);
            // controlRelay(relayIndex++, false);
            break;
        default:
            break;
        }
    }

    std::string hex = getHexCommand(onChannels);
    std::cout << hex << std::endl;
    telnet.sendCommand("relay writeall " + hex);
}

void RelayController::executeTransitionPhase()
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
    // int relayIndex = 1;
    std::vector<int> onChannels = {};

    for(const PhaseMessageType& p : transitionPhase)
    {

        switch(p)
        {
        case GREEN_PHASE:
            relayIndex++;
            onChannels.push_back(relayIndex++);
            relayIndex++;
            // controlRelay(relayIndex++, false);
            // controlRelay(relayIndex++, true);
            // controlRelay(relayIndex++, false);
            break;
        case RED_PHASE:
            onChannels.push_back(relayIndex++);
            relayIndex++;
            relayIndex++;
            // controlRelay(relayIndex++, true);
            // controlRelay(relayIndex++, false);
            // controlRelay(relayIndex++, false);
            break;
        case YELLOW_PHASE:
            relayIndex++;
            relayIndex++;
            onChannels.push_back(relayIndex++);
            // controlRelay(relayIndex++, true);
            // controlRelay(relayIndex++, false);
            // controlRelay(relayIndex++, false);
            break;
        case GREEN_PED:
            relayIndex++;
            onChannels.push_back(relayIndex++);
            // controlRelay(relayIndex++, false);
            // controlRelay(relayIndex++, true);
            break;
        case RED_PED:
            onChannels.push_back(relayIndex++);
            relayIndex++;
            // controlRelay(relayIndex++, true);
            // controlRelay(relayIndex++, false);
            break;
        default:
            break;
        }
    }

    std::string hex = getHexCommand(onChannels);
    std::cout << hex << std::endl;
    telnet.sendCommand("relay writeall " + hex);
}

std::vector<PhaseMessageType> RelayController::deriveTransitionPhase(
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

size_t RelayController::writeCallback(void* contents,
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

void RelayController::sendRequest(const std::string& url)
{
    if(curl)
    {
        std::string response;

        curl_easy_setopt(curl, CURLOPT_URL, (baseUrl + url).c_str());
        curl_easy_setopt(
            curl, CURLOPT_WRITEFUNCTION, RelayController::writeCallback);
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

void RelayController::turnOnRelay(int relayNumber)
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

void RelayController::turnOffRelay(int relayNumber)
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

void RelayController::controlRelay(int relayNumber, bool turnOn)
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
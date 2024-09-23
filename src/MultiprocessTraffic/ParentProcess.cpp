#include "ParentProcess.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <ctime>
#include <curl/curl.h>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

ParentProcess::ParentProcess(int numVehicle,
                             int numPedestrian,
                             std::vector<Pipe>& pipesParentToChild,
                             std::vector<Pipe>& pipesChildToParent,
                             std::vector<std::vector<PhaseMessageType>>& phases,
                             std::vector<int>& phaseDurations,
                             bool verbose,
                             float densityMultiplierGreenPhase,
                             float densityMultiplierRedPhase,
                             float densityMin,
                             float densityMax,
                             int minPhaseDurationMs,
                             int minPedestrianDurationMs,
                             std::string relayUrl)
    : numVehicle(numVehicle)
    , numPedestrian(numPedestrian)
    , pipesParentToChild(pipesParentToChild)
    , pipesChildToParent(pipesChildToParent)
    , phases(phases)
    , phaseDurations(phaseDurations)
    , verbose(verbose)
    , densityMultiplierGreenPhase(densityMultiplierGreenPhase)
    , densityMultiplierRedPhase(densityMultiplierRedPhase)
    , densityMin(densityMin)
    , densityMax(densityMax)
    , minPhaseDurationMs(minPhaseDurationMs)
    , minPedestrianDurationMs(minPedestrianDurationMs)
    , relayUrl(relayUrl)
{
    numChildren = numVehicle + numPedestrian;

    originalPhaseDurations = phaseDurations;

    fullCycleDurationMs = std::accumulate(
        originalPhaseDurations.begin(), originalPhaseDurations.end(), 0.0f);

    phaseRatio.clear();
    for(const auto& duration : originalPhaseDurations)
    {
        float ratio = static_cast<float>(duration) / fullCycleDurationMs;
        phaseRatio.push_back(ratio);
    }

    closeUnusedPipes();
}

// Function to URL-encode a string
std::string urlEncode1(const std::string& value)
{
    std::string encoded;
    char buf[4];
    for(unsigned char c : value)
    {
        if(std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            encoded += c;
        }
        else
        {
            std::sprintf(buf, "%%%02X", c);
            encoded += buf;
        }
    }
    return encoded;
}

// Function to decode a Base64 encoded string
std::string base64_decode1(const std::string& in)
{
    BIO *bio, *b64;
    std::string out;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(in.data(), in.size());
    bio = BIO_push(b64, bio);

    char buffer[512];
    int decodedSize = BIO_read(bio, buffer, sizeof(buffer));
    out.assign(buffer, decodedSize);

    BIO_free_all(bio);
    return out;
}

// Function to encode a string in Base64
std::string base64_encode1(const std::string& in)
{
    BIO *bio, *b64;
    BUF_MEM* bufferPtr;
    std::string out;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    BIO_write(bio, in.data(), in.size());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    out.assign(bufferPtr->data, bufferPtr->length - 1);
    out.push_back('\0'); // Ensure null-terminated string
    BIO_free_all(bio);

    return out;
}

// Function to generate a SAS token
std::string generateSASToken1(const std::string& resourceUri,
                              const std::string& key,
                              const std::string& policyName = "")
{
    // Calculate expiry time (1 day from now)
    time_t now = time(nullptr);
    int expiry = static_cast<int>(now + 24 * 3600); // 24 hours = 1 day

    // URL-encode the resource URI
    std::string encodedResourceUri = urlEncode1(resourceUri);

    // Create the string to sign
    std::string stringToSign =
        encodedResourceUri + "\n" + std::to_string(expiry);

    // Decode the key from Base64
    std::string decodedKey = base64_decode1(key);

    // Generate the HMAC-SHA256 signature
    unsigned char hmac[EVP_MAX_MD_SIZE];
    unsigned int hmacLength;
    HMAC(EVP_sha256(),
         reinterpret_cast<const unsigned char*>(decodedKey.c_str()),
         decodedKey.length(),
         reinterpret_cast<const unsigned char*>(stringToSign.c_str()),
         stringToSign.length(),
         hmac,
         &hmacLength);

    std::string signature =
        base64_encode1(std::string(reinterpret_cast<char*>(hmac), hmacLength));

    // Create the SAS token
    std::string token = "SharedAccessSignature sr=" + encodedResourceUri +
                        "&sig=" + urlEncode1(signature) +
                        "&se=" + std::to_string(expiry);

    if(!policyName.empty())
    {
        token += "&skn=" + urlEncode1(policyName);
    }

    return token;
}

void ParentProcess::sendMessageToIoTHub1(const std::string& message)
{
    CURL* curl;
    CURLcode res;

    // Connection string details
    std::string connectionString =
        "HostName=trafficez-hub.azure-devices.net;DeviceId=trafficez;"
        "SharedAccessKey=maEm8zZSYL3UV0wLlhQcvCicXKTmnlbRDAIoTBlsUcU=";
    std::string deviceId = "trafficez";
    // std::string message =
    //     R"({"name": "JH HUNT", "title": "mission impossible"})";

    // Extract Hostname and SharedAccessKey from connection string
    std::string hostname = "trafficez-hub.azure-devices.net";
    std::string sharedAccessKey =
        "maEm8zZSYL3UV0wLlhQcvCicXKTmnlbRDAIoTBlsUcU=";

    std::string resourceUri = "https://" + hostname + "/devices/" + deviceId;
    std::string sasToken = generateSASToken1(resourceUri, sharedAccessKey);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl)
    {
        std::string url = "https://" + hostname + "/devices/" + deviceId +
                          "/messages/deviceBound?api-version=2021-04-12";

        struct curl_slist* headers = nullptr;
        headers =
            curl_slist_append(headers, ("Authorization: " + sasToken).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message.c_str());

        res = curl_easy_perform(curl);

        if(res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: "
                      << curl_easy_strerror(res) << std::endl;
        }
        else if(verbose)
        {
            std::cout << "Message sent successfully to IOT HUB!" << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
}

void ParentProcess::run()
{
    relay.initialize(phases, relayUrl, verbose);

    int phaseIndex = 0;

    std::vector<std::vector<float>> phaseDensities(
        phases.size(), std::vector<float>(numChildren, 0.0));

    while(true)
    {
        if(verbose)
        {
            std::cout << "==================== Phase Cycle " << phaseIndex
                      << " ======================================\n";
        }

        relay.setPhaseCycle(phaseIndex);
        relay.executePhase();

        sendPhaseMessagesToChildren(phaseIndex);

        if(!receivePrevDensitiesFromChildren(phaseIndex, phaseDensities))
        {
            setDefaultPhaseDensities(phaseDensities);
        }

        handlePhaseTimer(phaseIndex);
        transitionToNextPhase(phaseIndex, phaseDensities);
    }

    for(int i = 0; i < numChildren; ++i)
    {
        wait(nullptr); // Wait for child processes to finish
    }
}

void ParentProcess::sendPhaseMessagesToChildren(int phaseIndex)
{
    for(int i = 0; i < numChildren; ++i)
    {
        if(verbose)
        {
            std::cout << "Parent: Sending phase message to child " << i << ": "
                      << phases[phaseIndex][i] << "\n";
        }

        std::string phaseString;
        switch(phases[phaseIndex][i])
        {
        case GREEN_PHASE:
            phaseString = "GREEN_PHASE";
            break;
        case RED_PHASE:
            phaseString = "RED_PHASE";
            break;
        case GREEN_PED:
            phaseString = "GREEN_PED";
            break;
        case RED_PED:
            phaseString = "RED_PED";
            break;
        default:
            phaseString = "UNKNOWN";
            break;
        }

        if(write(pipesParentToChild[i].fds[1],
                 phaseString.c_str(),
                 phaseString.size() + 1) == -1)
        {
            std::cerr << "Parent: Failed to write to pipe: " << strerror(errno)
                      << "\n";
            break;
        }
    }
}

bool ParentProcess::receivePrevDensitiesFromChildren(
    int phaseIndex, std::vector<std::vector<float>>& phaseDensities)
{
    char buffer[BUFFER_SIZE];

    int previousPhaseIndex =
        (phaseIndex == 0) ? phases.size() - 1 : phaseIndex - 1;

    for(int i = 0; i < numChildren; ++i)
    {
        float density;
        if(!readDensityFromChild(i, density))
        {
            return false;
        }

        PhaseMessageType previousPhaseType = phases[previousPhaseIndex][i];
        processDensityByPhaseType(previousPhaseType, density);
        density = std::clamp(density, densityMin, densityMax);

        if(verbose)
        {
            std::cout << "Previous child " << i << " data: " << density << "\n";
        }

        phaseDensities[previousPhaseIndex][i] = density;
    }

    return true;
}

bool ParentProcess::readDensityFromChild(int childIndex, float& density)
{
    char buffer[BUFFER_SIZE];
    int bytesRead =
        read(pipesChildToParent[childIndex].fds[0], buffer, sizeof(buffer) - 1);
    if(bytesRead <= 0)
    {
        std::cerr << "Parent: Failed to read from pipe or EOF reached: "
                  << strerror(errno) << "\n";
        return false;
    }
    buffer[bytesRead] = '\0'; // Ensure null termination

    try
    {
        density = std::stof(buffer);
    }
    catch(const std::invalid_argument&)
    {
        std::cerr << "Parent: Invalid density value received from child "
                  << childIndex << "\n";
        return false;
    }

    if(std::isnan(density) || (std::isnan(density) && std::signbit(density)))
    {
        std::cerr << "Parent: Detected NaN or negative NaN in traffic density "
                     "from child "
                  << childIndex << "\n";
        return false;
    }

    return true;
}

void ParentProcess::processDensityByPhaseType(PhaseMessageType phaseType,
                                              float& density)
{
    switch(phaseType)
    {
    case GREEN_PHASE:
        density *= densityMultiplierGreenPhase;
        break;
    case RED_PHASE:
        density = (densityMax - density) * densityMultiplierRedPhase;
        break;
    case RED_PED:
        density = 0;
        break;
    case GREEN_PED:
        // No specific handling needed
        break;
    default:
        break;
    }
}

void ParentProcess::handlePhaseTimer(int phaseIndex)
{
    int remainingTime = phaseDurations[phaseIndex] / 1000;

    while(remainingTime > 0)
    {
        std::cout << "\rRemaining time for phase " << phaseIndex << ": "
                  << remainingTime << " seconds.";
        std::cout.flush();
        sleep(1);

        if(remainingTime == 5)
        {
            relay.executeTransitionPhase();
        }

        --remainingTime;
    }

    std::cout << std::endl;
}

void ParentProcess::transitionToNextPhase(
    int& phaseIndex, std::vector<std::vector<float>>& phaseDensities)
{
    phaseIndex = (phaseIndex + 1) % phases.size();

    if(phaseIndex == 0)
    {
        updatePhaseDurations(phaseDensities);
    }
}

void ParentProcess::setDefaultPhaseDensities(
    std::vector<std::vector<float>>& phaseDensities)
{
    if(phaseDensities.size() != phaseRatio.size())
    {
        std::cerr << "Size of phase density and ratio do not match!\n";
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < phaseRatio.size(); ++i)
    {
        phaseDensities[i] = std::vector<float>(numChildren, phaseRatio[i]);
    }

    std::cout << "Parent: Phase densities set to default values.\n";
}

void ParentProcess::updatePhaseDurations(
    const std::vector<std::vector<float>>& phaseDensities)
{
    float totalDensity = 0.0;
    std::vector<float> phaseTotals(phases.size(), 0.0);
    std::vector<float> pedestrianTotals(phases.size(), 0.0);

    if(verbose)
    {
        std::cout << "----- Density Distribution ------------\n";
    }

    for(int phase = 0; phase < phases.size(); ++phase)
    {
        for(int child = 0; child < numVehicle; ++child)
        {
            phaseTotals[phase] += phaseDensities[phase][child];

            if(verbose)
            {
                std::cout << "Phase " << phase << " - child " << child
                          << " density: " << phaseDensities[phase][child]
                          << "\n";
            }
        }
        totalDensity += phaseTotals[phase];

        for(int child = numVehicle; child < numChildren; ++child)
        {
            pedestrianTotals[phase] += phaseDensities[phase][child];

            if(verbose)
            {
                std::cout << "Phase " << phase << " - child " << child
                          << " pedestrian: " << phaseDensities[phase][child]
                          << "\n";
            }
        }
    }

    std::cout << "----------------------------------------------------------\n";

    // Update phase durations based on the density ratios
    bool validDurations = true;
    for(int phase = 0; phase < phases.size(); ++phase)
    {
        phaseDurations[phase] = static_cast<int>(
            (totalDensity == 0.0 ? 1.0 : phaseTotals[phase] / totalDensity) *
            fullCycleDurationMs);

        // so we always have enough time for yellow
        if(phaseDurations[phase] < minPhaseDurationMs)
        {
            phaseDurations[phase] = minPhaseDurationMs;
        }

        if(phaseDurations[phase] > fullCycleDurationMs)
        {
            validDurations = false;
            break;
        }

        // if there is waiting pedestrian, prioritize them
        if(pedestrianTotals[phase] > 0 &&
           phaseDurations[phase] < minPedestrianDurationMs)
        {
            phaseDurations[phase] = minPedestrianDurationMs;
        }

        std::cout << "Phase " << phase
                  << " allocated time: " << phaseDurations[phase] / 1000.0
                  << " seconds.\n";
    }

    std::cout << "----------------------------------------------------------\n";

    if(!validDurations)
    {
        phaseDurations = originalPhaseDurations;
        std::cerr << "Parent: Phase durations set to original values due to "
                     "invalid duration.\n";
    }
}

void ParentProcess::closeUnusedPipes()
{
    for(int i = 0; i < numChildren; ++i)
    {
        close(pipesParentToChild[i].fds[0]);
        close(pipesChildToParent[i].fds[1]);
    }
}
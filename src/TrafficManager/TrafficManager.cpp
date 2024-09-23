#include "TrafficManager.h"
#include "MultiprocessTraffic.h"
#include "WatcherSpawner.h"
#include <ctime>
#include <curl/curl.h>
#include <iomanip>
#include <iostream>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <sstream>
#include <string>

TrafficManager::TrafficManager(const std::string& configFile,
                               bool debug,
                               bool calib,
                               bool verbose,
                               bool test)
    : configFile(configFile)
    , debugMode(debug)
    , calibMode(calib)
    , verbose(verbose)
    , testMode(test)
{}

// Function to URL-encode a string
std::string urlEncode(const std::string& value)
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
std::string base64_decode(const std::string& in)
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
std::string base64_encode(const std::string& in)
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
std::string generateSASToken(const std::string& resourceUri,
                             const std::string& key,
                             const std::string& policyName = "")
{
    // Calculate expiry time (1 day from now)
    time_t now = time(nullptr);
    int expiry = static_cast<int>(now + 24 * 3600); // 24 hours = 1 day

    // URL-encode the resource URI
    std::string encodedResourceUri = urlEncode(resourceUri);

    // Create the string to sign
    std::string stringToSign =
        encodedResourceUri + "\n" + std::to_string(expiry);

    // Decode the key from Base64
    std::string decodedKey = base64_decode(key);

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
        base64_encode(std::string(reinterpret_cast<char*>(hmac), hmacLength));

    // Create the SAS token
    std::string token = "SharedAccessSignature sr=" + encodedResourceUri +
                        "&sig=" + urlEncode(signature) +
                        "&se=" + std::to_string(expiry);

    if(!policyName.empty())
    {
        token += "&skn=" + urlEncode(policyName);
    }

    return token;
}

void TrafficManager::sendMessageToIoTHub(const std::string& message)
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
    std::string sasToken = generateSASToken(resourceUri, sharedAccessKey);

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
        else
        {
            std::cout << "Message sent successfully to IOT HUB!" << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
}

void TrafficManager::start()
{
    if(testMode)
    {
        test();
        std::cout << "\nTest Successful!!\n";
        exit(EXIT_SUCCESS);
    }

    std::cout << "TrafficManager starting...\n";
    std::cout << "Debug Mode: " << (debugMode ? "true" : "false") << "\n";
    std::cout << "Calib Mode: " << (calibMode ? "true" : "false") << "\n";
    std::cout << "Verbose Mode: " << (verbose ? "true" : "false") << "\n";

    MultiprocessTraffic multiprocessTraffic(configFile, debugMode, verbose);

    if(calibMode)
        multiprocessTraffic.calibrate();
    else
        multiprocessTraffic.start();

    std::cout << "TrafficManager ended.\n";
}

void TrafficManager::test()
{
    initTestVariables();

    int greenFramesToCheck = 100;
    int redFramesToCheck = 20;

    testVehicleWatcherGui(greenFramesToCheck, redFramesToCheck);
    testVehicleWatcherHeadless(greenFramesToCheck, redFramesToCheck);
    compareVehicleResults();

    testPedestrianWatcherGui(redFramesToCheck);
    testPedestrianWatcherHeadless(redFramesToCheck);
    comparePedestrianResults();
}

void TrafficManager::initTestVariables()
{
    greenCountGui = 0;
    greenDensityGui = 0.0;
    redCountGui = 0;
    redDensityGui = 0.0;
    pedCountGui = 0;
    greenCountHeadless = 0;
    greenDensityHeadless = 0.0;
    redCountHeadless = 0;
    redDensityHeadless = 0.0;
    pedCountHeadless = 0;
}

void TrafficManager::testVehicleWatcherGui(int greenFramesToCheck,
                                           int redFramesToCheck)
{
    WatcherSpawner spawner;
    Watcher* vehicleWatcher = spawner.spawnWatcher(WatcherType::VEHICLE,
                                                   RenderMode::GUI,
                                                   "testVehicle.mp4",
                                                   "testVehicle.yaml");

    std::cout << "\nStarting Vehicle GUI mode...\n";

    vehicleWatcher->setCurrentTrafficState(TrafficState::GREEN_PHASE);
    for(int currFrame = 0; currFrame < greenFramesToCheck; ++currFrame)
    {
        vehicleWatcher->processFrame();
    }

    greenCountGui = vehicleWatcher->getInstanceCount();
    greenDensityGui = vehicleWatcher->getTrafficDensity();

    std::cout << "Green Vehicle count: " << greenCountGui << "\n";
    std::cout << "Green Vehicle density: " << greenDensityGui << "\n";

    if(greenCountGui <= 0 || greenDensityGui <= 0)
    {
        std::cerr << "Vehicle Detection failed on green phase\n";
        exit(EXIT_FAILURE);
    }

    vehicleWatcher->setCurrentTrafficState(TrafficState::RED_PHASE);
    for(int currFrame = 0; currFrame < redFramesToCheck; ++currFrame)
    {
        vehicleWatcher->processFrame();
    }

    redCountGui = vehicleWatcher->getInstanceCount();
    redDensityGui = vehicleWatcher->getTrafficDensity();

    std::cout << "Red Vehicle count: " << redCountGui << "\n";
    std::cout << "Red Vehicle density: " << redDensityGui << "\n";

    if(redCountGui <= 0 || redDensityGui <= 0)
    {
        std::cerr << "Vehicle Detection failed on red phase\n";
        exit(EXIT_FAILURE);
    }

    delete vehicleWatcher;
}

void TrafficManager::testVehicleWatcherHeadless(int greenFramesToCheck,
                                                int redFramesToCheck)
{
    WatcherSpawner spawner;
    Watcher* vehicleWatcher = spawner.spawnWatcher(WatcherType::VEHICLE,
                                                   RenderMode::HEADLESS,
                                                   "testVehicle.mp4",
                                                   "testVehicle.yaml");

    std::cout << "\nStarting Vehicle Headless mode...\n";

    vehicleWatcher->setCurrentTrafficState(TrafficState::GREEN_PHASE);
    for(int currFrame = 0; currFrame < greenFramesToCheck; ++currFrame)
    {
        vehicleWatcher->processFrame();
    }

    greenCountHeadless = vehicleWatcher->getInstanceCount();
    greenDensityHeadless = vehicleWatcher->getTrafficDensity();

    std::cout << "Green Vehicle count: " << greenCountHeadless << "\n";
    std::cout << "Green Vehicle density: " << greenDensityHeadless << "\n";

    if(greenCountHeadless <= 0 || greenDensityHeadless <= 0)
    {
        std::cerr << "Vehicle Detection failed on green phase\n";
        exit(EXIT_FAILURE);
    }

    vehicleWatcher->setCurrentTrafficState(TrafficState::RED_PHASE);
    for(int currFrame = 0; currFrame < redFramesToCheck; ++currFrame)
    {
        vehicleWatcher->processFrame();
    }

    redCountHeadless = vehicleWatcher->getInstanceCount();
    redDensityHeadless = vehicleWatcher->getTrafficDensity();

    std::cout << "Red Vehicle count: " << redCountHeadless << "\n";
    std::cout << "Red Vehicle density: " << redDensityHeadless << "\n";

    if(redCountHeadless <= 0 || redDensityHeadless <= 0)
    {
        std::cerr << "Vehicle Detection failed on red phase\n";
        exit(EXIT_FAILURE);
    }

    delete vehicleWatcher;
}

void TrafficManager::compareVehicleResults()
{
    if(greenCountHeadless != greenCountGui)
    {
        std::cerr << "Vehicle Count for GUI and Headless mode have different "
                     "results for green phase!\n";
        exit(EXIT_FAILURE);
    }

    if(redCountHeadless != redCountGui)
    {
        std::cerr << "Vehicle Count for GUI and Headless mode have different "
                     "results for red phase!\n";
        exit(EXIT_FAILURE);
    }

    float densityErrorMargin = 1.0;

    if(std::abs(redDensityHeadless - redDensityGui) > densityErrorMargin)
    {
        std::cerr << "Vehicle Density for GUI and Headless mode have different "
                     "results for red phase!\n";
        exit(EXIT_FAILURE);
    }

    // REMOVED because mp4 test playback is faster, meaning that time calculation
    // for GUI and Headless mode is different.
    // if(std::abs(greenDensityHeadless - greenDensityGui) > densityErrorMargin)
    // {
    //     std::cerr << "Vehicle Density for GUI and Headless mode have different "
    //                  "results for green phase!\n";
    //     exit(EXIT_FAILURE);
    // }
}

void TrafficManager::testPedestrianWatcherGui(int redFramesToCheck)
{
    WatcherSpawner spawner;
    Watcher* pedestrianWatcher = spawner.spawnWatcher(WatcherType::PEDESTRIAN,
                                                      RenderMode::GUI,
                                                      "testPedestrian.mp4",
                                                      "testPedestrian.yaml");

    std::cout << "\nStarting Pedestrian GUI mode...\n";

    for(int currFrame = 0; currFrame < redFramesToCheck; ++currFrame)
    {
        pedestrianWatcher->processFrame();
    }

    pedCountGui = pedestrianWatcher->getInstanceCount();

    std::cout << "GUI Pedestrian count: " << pedCountGui << "\n";

    if(pedCountGui <= 0)
    {
        std::cerr << "Pedestrian Detection failed\n";
        exit(EXIT_FAILURE);
    }

    delete pedestrianWatcher;
}

void TrafficManager::testPedestrianWatcherHeadless(int redFramesToCheck)
{
    WatcherSpawner spawner;
    Watcher* pedestrianWatcher = spawner.spawnWatcher(WatcherType::PEDESTRIAN,
                                                      RenderMode::HEADLESS,
                                                      "testPedestrian.mp4",
                                                      "testPedestrian.yaml");

    std::cout << "\nStarting Pedestrian Headless mode...\n";

    for(int currFrame = 0; currFrame < redFramesToCheck; ++currFrame)
    {
        pedestrianWatcher->processFrame();
    }

    pedCountHeadless = pedestrianWatcher->getInstanceCount();

    std::cout << "Headless Pedestrian count: " << pedCountHeadless << "\n";

    if(pedCountHeadless <= 0)
    {
        std::cerr << "Pedestrian Detection failed\n";
        exit(EXIT_FAILURE);
    }

    delete pedestrianWatcher;
}

void TrafficManager::comparePedestrianResults()
{
    if(pedCountHeadless != pedCountGui)
    {
        std::cerr << "Pedestrian Count for GUI and Headless mode have "
                     "different results!\n";
        exit(EXIT_FAILURE);
    }
}

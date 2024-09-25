#ifndef IOT_HUB_CLIENT_H
#define IOT_HUB_CLIENT_H

#include <string>
#include <curl/curl.h>
#include <openssl/hmac.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <ctime>
#include <iostream>

class IotHubClient
{
public:
    IotHubClient() {
        loadIotConfig();
    }

    void sendMessageToIoTHub(const std::string& message);

private:
    std::string iConfigFile = "iothub_config.yaml";
    std::string hostname;
    std::string deviceId;
    std::string sharedAccessKey;

    static std::string urlEncode(const std::string& value);
    static std::string base64Decode(const std::string& in);
    static std::string base64Encode(const std::string& in);
    static std::string generateSASToken(const std::string& resourceUri, 
                                        const std::string& key, 
                                        const std::string& policyName = "");

    void loadIotConfig();

};

#endif

#ifndef IOT_HUB_CLIENT_H
#define IOT_HUB_CLIENT_H

#include <string>

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
    std::string sharedAccessKeyName;
    std::string sharedAccessKey;
    unsigned char* digest;

    static std::string urlEncode(const std::string& value);
    static std::string base64Decode(const std::string& in);
    static std::string base64Encode(const unsigned char* input, int length);
    static std::string base64EncodeForSas(const std::string& input);
    static std::string generateSASToken(const std::string& resourceUri, 
                                        const std::string& key, 
                                        const std::string& policyName);

    void loadIotConfig();

};

#endif

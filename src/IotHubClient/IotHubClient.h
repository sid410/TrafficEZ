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
    static std::string urlEncode(const std::string& value);
    static std::string base64_decode(const std::string& in);
    static std::string base64_encode(const std::string& in);
    static std::string generateSASToken(const std::string& resourceUri, const std::string& key, const std::string& policyName = "");
    void sendMessageToIoTHub(const std::string& message);
};

#endif // IOT_HUB_CLIENT_H

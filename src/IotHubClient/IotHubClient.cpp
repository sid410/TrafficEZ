#include "IotHubClient.h"
#include <ctime>
#include <curl/curl.h>
#include <iostream>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/hmac.h>
#include <yaml-cpp/yaml.h>

// Function to URL-encode a string
std::string IotHubClient::urlEncode(const std::string& value)
{
    CURL* curl = curl_easy_init();
    char* output = curl_easy_escape(curl, value.c_str(), value.length());
    std::string result(output);
    curl_free(output);
    curl_easy_cleanup(curl);
    return result;
}

// Function to decode a Base64 encoded string
std::string IotHubClient::base64Decode(const std::string& in)
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
std::string IotHubClient::base64Encode(const unsigned char* input, int length)
{
    BIO* bmem = NULL;
    BIO* b64 = NULL;
    BUF_MEM* bptr = NULL;

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // Do not add newlines
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, input, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    std::string output(bptr->data, bptr->length);
    BIO_free_all(b64);

    return output;
}

// Wrapper to encode std::string in Base64 for SAS token generation
std::string IotHubClient::base64EncodeForSas(const std::string& input)
{
    return base64Encode(reinterpret_cast<const unsigned char*>(input.c_str()),
                        input.size());
}

// Function to generate a SAS token
std::string IotHubClient::generateSASToken(const std::string& resourceUri,
                                           const std::string& key,
                                           const std::string& policyName = "")
{
    // Calculate expiry time (1 day from now)
    time_t now = time(nullptr); // Current time
    time_t expiry = now + 24 * 3600; // 24 hours = 1 day

    // Testing the system and IoT Hub time synchronization
    std::cout << "Current time: " << ctime(&now);
    std::cout << "Expiry time: " << ctime(&expiry);

    // Create the string to sign (do not URL-encode here)
    std::string stringToSign = resourceUri + "\n" + std::to_string(expiry);

    // Decode the key from Base64
    std::string decodedKey = base64Decode(key);

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

    // Base64-encode the signature
    std::string signature = base64EncodeForSas(
        std::string(reinterpret_cast<char*>(hmac), hmacLength));

    // URL-encode the resource URI and the signature
    std::string encodedResourceUri = urlEncode(resourceUri);
    std::string encodedSignature = urlEncode(signature);

    // Create the SAS token
    std::string token = "SharedAccessSignature sr=" + encodedResourceUri +
                        "&sig=" + encodedSignature +
                        "&se=" + std::to_string(expiry);

    if(!policyName.empty())
    {
        token += "&skn=" + urlEncode(policyName);
    }

    return token;
}

// Function to send a message to IoT Hub
void IotHubClient::sendMessageToIoTHub(const std::string& message)
{
    CURL* curl;
    CURLcode res;

    std::string resourceUri = "https://" + hostname + "/devices/" + deviceId;
    std::string sasToken =
        generateSASToken(resourceUri, sharedAccessKey, sharedAccessKeyName);
    std::cout << "SAS Token: " << sasToken << "\n\n";

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl)
    {
        // std::string url = "https://" + hostname + "/devices/" + deviceId + "/messages/events?api-version=2021-04-12";
        // std::string url = "https://" + hostname + "/devices/" + deviceId + "/messages/devicebound?api-version=2021-04-12";
        std::string url = "https://" + hostname + "/devices/" + deviceId +
                          "/messages/devicebound?api-version=2020-09-30";
        std::cout << "Sending message to URL: " << url << std::endl;

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
            std::cerr << "Failed to send message: " << curl_easy_strerror(res)
                      << std::endl;
        }
        else
        {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            if(response_code == 200 || response_code == 204)
            {
                std::cout << "Message sent successfully!" << std::endl;
            }
            else
            {
                std::cerr << "Error: Received HTTP " << response_code
                          << " from IoT Hub." << std::endl;
                if(response_code == 401)
                {
                    std::cerr << "Unauthorized - Check SAS token!" << std::endl;
                }
                else if(response_code == 403)
                {
                    std::cerr << "Forbidden - Check permissions!" << std::endl;
                }
            }
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
}

void IotHubClient::loadIotConfig()
{
    try
    {
        // Load YAML file
        YAML::Node config = YAML::LoadFile(iConfigFile);

        // Check if required parameters are available
        if(!config["hostname"] || !config["deviceId"] ||
           !config["sharedAccessKey"])
        {
            throw std::runtime_error("Missing required IoT Hub configuration "
                                     "parameters in YAML file.");
        }

        // Retrieve required parameters from YAML file
        hostname = config["hostname"].as<std::string>();
        deviceId = config["deviceId"].as<std::string>();
        sharedAccessKey = config["sharedAccessKey"].as<std::string>();

        // Check if the optional parameter sharedAccessKeyName is present
        if(config["sharedAccessKeyName"])
        {
            sharedAccessKeyName =
                config["sharedAccessKeyName"].as<std::string>();
        }
        else
        {
            // Optionally assign a default value or handle the absence of this parameter
            sharedAccessKeyName =
                ""; // Set to an empty string or handle accordingly
            // std::cout << "Warning: sharedAccessKeyName is not provided in the configuration file, using default value." << std::endl;
        }

        std::string connectionString =
            "HostName=" + hostname + ";DeviceId=" + deviceId +
            ";SharedAccessKeyName=" + sharedAccessKeyName +
            ";SharedAccessKey=" + sharedAccessKey + "\n";

        // Checking the fetched parameters
        // std::cout << "Connection string: " << connectionString;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error loading IoT configuration from YAML: " << e.what()
                  << std::endl;
    }
}

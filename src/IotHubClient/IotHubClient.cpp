#include "IotHubClient.h"

// Function to URL-encode a string
std::string IotHubClient::urlEncode(const std::string& value)
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
std::string IotHubClient::base64_decode(const std::string& in)
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
std::string IotHubClient::base64_encode(const std::string& in)
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
std::string IotHubClient::generateSASToken(const std::string& resourceUri,
                                           const std::string& key,
                                           const std::string& policyName)
{
    // Calculate expiry time (1 day from now)
    time_t now = time(nullptr);
    int expiry = static_cast<int>(now + 24 * 3600); // 24 hours = 1 day

    // URL-encode the resource URI
    std::string encodedResourceUri = urlEncode(resourceUri);

    // Create the string to sign
    std::string stringToSign = encodedResourceUri + "\n" + std::to_string(expiry);

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

    std::string signature = base64_encode(std::string(reinterpret_cast<char*>(hmac), hmacLength));

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

// Function to send message to IoT Hub
void IotHubClient::sendMessageToIoTHub(const std::string& message)
{
    CURL* curl;
    CURLcode res;

    // Connection string details
    std::string connectionString = "HostName=trafficez-hub.azure-devices.net;DeviceId=trafficez;SharedAccessKey=maEm8zZSYL3UV0wLlhQcvCicXKTmnlbRDAIoTBlsUcU=";
    std::string deviceId = "trafficez";

    // Extract Hostname and SharedAccessKey from connection string
    std::string hostname = "trafficez-hub.azure-devices.net";
    std::string sharedAccessKey = "maEm8zZSYL3UV0wLlhQcvCicXKTmnlbRDAIoTBlsUcU=";

    std::string resourceUri = "https://" + hostname + "/devices/" + deviceId;
    std::string sasToken = generateSASToken(resourceUri, sharedAccessKey);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl)
    {
        std::string url = "https://" + hostname + "/devices/" + deviceId + "/messages/deviceBound?api-version=2021-04-12";

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: " + sasToken).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message.c_str());

        res = curl_easy_perform(curl);

        if(res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            std::cout << "Message sent successfully to IoT Hub!" << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
}

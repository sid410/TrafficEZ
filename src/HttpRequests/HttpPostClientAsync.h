#ifndef HTTP_POST_CLIENT_ASYNC_H
#define HTTP_POST_CLIENT_ASYNC_H

#include <curl/curl.h>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <thread>

class HttpPostClientAsync
{
public:
    HttpPostClientAsync();
    ~HttpPostClientAsync();

    // Sends a POST request asynchronously to the specified URL with the given data and optional headers.
    void sendPostRequestAsync(
        const std::string& url,
        const std::string& data,
        const std::map<std::string, std::string>& headers = {},
        std::function<void(bool, int, const std::string&)> callback = nullptr);

private:
    void cleanupCurl();
    void
    performAsync(CURL* curlHandle,
                 CURLM* multi_handle,
                 std::string& responseBuffer,
                 std::function<void(bool, int, const std::string&)> callback);

    static size_t writeCallback(void* contents,
                                size_t size,
                                size_t nmemb,
                                void* userp); // Corrected signature

    std::mutex curlMutex; // Mutex to ensure thread safety
    CURLM* multi_handle; // Multi handle for asynchronous requests
};

#endif // HTTP_POST_CLIENT_ASYNC_H

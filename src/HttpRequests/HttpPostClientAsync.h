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
    void
    sendPostRequestAsync(const std::string& url,
                         const std::string& data,
                         const std::map<std::string, std::string>& headers = {},
                         std::function<void(bool)> callback = nullptr);

private:
    void cleanupCurl();
    void performAsync(std::function<void(bool)> callback);

    CURL* curl; // Pointer to the CURL instance
    CURLM* multi_handle; // Multi handle for asynchronous requests
    std::thread worker; // Thread to perform the async request
    std::mutex curl_mutex; // Mutex for thread safety
};

#endif // HTTP_POST_CLIENT_ASYNC_H

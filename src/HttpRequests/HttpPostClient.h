#ifndef HTTP_POST_CLIENT_H
#define HTTP_POST_CLIENT_H

#include <curl/curl.h>
#include <map>
#include <string>

class HttpPostClient
{
public:
    HttpPostClient();
    ~HttpPostClient();

    // Sends a POST request to the specified URL with the given data and optional headers.
    bool
    sendPostRequest(const std::string& url,
                    const std::string& data,
                    const std::map<std::string, std::string>& headers = {});

private:
    void cleanupCurl();
    CURL* curl; // Pointer to the CURL instance
};

#endif // HTTP_POST_CLIENT_H

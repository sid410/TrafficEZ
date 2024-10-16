#include "HttpPostClient.h"
#include <iostream>

HttpPostClient::HttpPostClient()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
}

HttpPostClient::~HttpPostClient()
{
    cleanupCurl();
    curl_global_cleanup();
}

bool HttpPostClient::sendPostRequest(
    const std::string& url,
    const std::string& data,
    const std::map<std::string, std::string>& headers)
{
    if(!curl)
    {
        std::cerr << "CURL not initialized." << std::endl;
        return false;
    }

    struct curl_slist* header_list = nullptr;
    for(const auto& header : headers)
    {
        std::string header_entry = header.first + ": " + header.second;
        header_list = curl_slist_append(header_list, header_entry.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

    CURLcode res = curl_easy_perform(curl);

    curl_slist_free_all(header_list); // Free the list of headers

    if(res != CURLE_OK)
    {
        std::cerr << "CURL request failed: " << curl_easy_strerror(res)
                  << std::endl;
        return false;
    }

    return true;
}

void HttpPostClient::cleanupCurl()
{
    if(curl)
    {
        curl_easy_cleanup(curl);
        curl = nullptr;
    }
}
#include "HttpPostClientAsync.h"
#include <cstring>
#include <iostream>

HttpPostClientAsync::HttpPostClientAsync()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    multi_handle = curl_multi_init();
}

HttpPostClientAsync::~HttpPostClientAsync()
{
    cleanupCurl();
    curl_global_cleanup();
}

void HttpPostClientAsync::sendPostRequestAsync(
    const std::string& url,
    const std::string& data,
    const std::map<std::string, std::string>& headers,
    std::function<void(bool, int, const std::string&)> callback)
{
    // Lock to ensure thread safety
    std::lock_guard<std::mutex> lock(curlMutex);

    // Initialize a new CURL handle for this request
    CURL* curlHandle = curl_easy_init();
    if(!curlHandle || !multi_handle)
    {
        std::cerr << "CURL not initialized." << std::endl;
        if(callback)
            callback(false, 0, "CURL initialization failed.");
        return;
    }

    // Set up headers
    struct curl_slist* header_list = nullptr;
    for(const auto& header : headers)
    {
        std::string header_entry = header.first + ": " + header.second;
        header_list = curl_slist_append(header_list, header_entry.c_str());
    }

    // Set CURL options
    curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, header_list);

    // Prepare the response buffer
    std::string responseBuffer;

    // Set the write callback
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curlHandle,
                     CURLOPT_WRITEDATA,
                     &responseBuffer); // Pass the response buffer
    curl_easy_setopt(curlHandle, CURLOPT_TIMEOUT, 10L); // 10 seconds

    // Add the easy handle to the multi handle
    curl_multi_add_handle(multi_handle, curlHandle);

    // Create a thread to handle the async request
    std::thread worker(
        &HttpPostClientAsync::performAsync,
        this,
        curlHandle,
        multi_handle,
        std::ref(responseBuffer), // Use std::ref to pass by reference
        callback);

    // Detach the thread so it can run independently
    worker.detach();
}

size_t HttpPostClientAsync::writeCallback(void* contents,
                                          size_t size,
                                          size_t nmemb,
                                          void* userp)
{
    // The 'userp' parameter should point to the std::string buffer
    std::string* out = static_cast<std::string*>(userp);
    size_t totalSize =
        size * nmemb; // Calculate the total size of incoming data
    out->append(static_cast<char*>(contents),
                totalSize); // Append the data to the output string
    return totalSize; // Return the size of the processed data
}

void HttpPostClientAsync::performAsync(
    CURL* curlHandle,
    CURLM* multi_handle,
    std::string& responseBuffer,
    std::function<void(bool, int, const std::string&)> callback)
{
    int still_running = 0;
    CURLMcode mc;

    do
    {
        mc = curl_multi_perform(multi_handle, &still_running);

        if(mc != CURLM_OK)
        {
            std::cerr << "CURLM perform error: " << curl_multi_strerror(mc)
                      << std::endl;
            if(callback)
                callback(false, static_cast<int>(mc), "CURLM perform error.");
            break;
        }

        // Wait for activity with a timeout to avoid busy-looping
        int numfds = 0;
        mc = curl_multi_wait(multi_handle, nullptr, 0, 1000, &numfds);
        if(mc != CURLM_OK)
        {
            std::cerr << "CURLM wait error: " << curl_multi_strerror(mc)
                      << std::endl;
            if(callback)
                callback(false, static_cast<int>(mc), "CURLM wait error.");
            break;
        }

        // Simulate doing some work while waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    } while(still_running);

    // Once done, check for errors and call the callback
    CURLMsg* msg;
    int msgs_left;
    while((msg = curl_multi_info_read(multi_handle, &msgs_left)))
    {
        if(msg->msg == CURLMSG_DONE)
        {
            if(msg->data.result == CURLE_OK)
            {
                if(callback)
                    callback(true, 0, responseBuffer); // Pass response
            }
            else
            {
                std::cerr << "CURL request failed: "
                          << curl_easy_strerror(msg->data.result) << std::endl;
                if(callback)
                    callback(false,
                             static_cast<int>(msg->data.result),
                             curl_easy_strerror(
                                 msg->data.result)); // Pass error message
            }
        }
    }

    // Clean up the easy handle after completion
    curl_multi_remove_handle(multi_handle, curlHandle);
    curl_easy_cleanup(curlHandle);
}

void HttpPostClientAsync::cleanupCurl()
{
    if(multi_handle)
    {
        curl_multi_cleanup(multi_handle);
        multi_handle = nullptr;
    }
}

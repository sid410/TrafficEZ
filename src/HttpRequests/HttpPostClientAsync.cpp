#include "HttpPostClientAsync.h"
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

HttpPostClientAsync::HttpPostClientAsync()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
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
    std::function<void(bool)> callback)
{
    std::lock_guard<std::mutex> lock(curl_mutex); // Protecting shared resources
    if(!curl || !multi_handle)
    {
        std::cerr << "CURL not initialized." << std::endl;
        if(callback)
            callback(false);
        return;
    }

    // Create and set up the header list
    struct curl_slist* header_list = nullptr;
    for(const auto& header : headers)
    {
        std::string header_entry = header.first + ": " + header.second;
        header_list = curl_slist_append(header_list, header_entry.c_str());
    }

    // Set CURL options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

    // Add the easy handle to the multi handle
    curl_multi_add_handle(multi_handle, curl);

    // Start a new thread to handle the async operation
    worker = std::thread(&HttpPostClientAsync::performAsync, this, callback);

    // Detach the thread to allow it to run independently
    worker.detach();

    // Free the header list after adding it to CURL
    curl_slist_free_all(header_list);
}

void HttpPostClientAsync::performAsync(std::function<void(bool)> callback)
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
                callback(false);
            return; // Exit on error
        }

        // Wait for activity, with a timeout to avoid busy-looping
        int numfds = 0;
        mc = curl_multi_wait(multi_handle, nullptr, 0, 1000, &numfds);
        if(mc != CURLM_OK)
        {
            std::cerr << "CURLM wait error: " << curl_multi_strerror(mc)
                      << std::endl;
            if(callback)
                callback(false);
            return; // Exit on error
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
                    callback(true);
            }
            else
            {
                std::cerr << "CURL request failed: "
                          << curl_easy_strerror(msg->data.result) << std::endl;
                if(callback)
                    callback(false);
            }
        }
    }

    // Clean up the easy handle after completion
    curl_multi_remove_handle(multi_handle, curl);
}

void HttpPostClientAsync::cleanupCurl()
{
    std::lock_guard<std::mutex> lock(curl_mutex); // Protecting cleanup
    if(curl)
    {
        curl_easy_cleanup(curl);
        curl = nullptr;
    }

    if(multi_handle)
    {
        curl_multi_cleanup(multi_handle);
        multi_handle = nullptr;
    }
}

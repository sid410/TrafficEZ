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
    const nlohmann::json& data,
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
        struct curl_slist* temp_list =
            curl_slist_append(header_list, header_entry.c_str());
        if(!temp_list) // Check for allocation failure
        {
            curl_slist_free_all(header_list);
            curl_easy_cleanup(curlHandle);
            std::cerr << "Failed to allocate headers." << std::endl;
            if(callback)
                callback(false, 0, "Header allocation failed.");
            return;
        }
        header_list = temp_list;
    }

    // Set CURL options
    std::string serializedData = data.dump(); // Serialize JSON to string
    curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, serializedData.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, header_list);

    // Prepare the response buffer
    std::string responseBuffer;

    // Set the write callback
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &responseBuffer);
    curl_easy_setopt(curlHandle, CURLOPT_TIMEOUT_MS, 5000L);

    // Add the easy handle to the multi handle
    CURLMcode result = curl_multi_add_handle(multi_handle, curlHandle);
    if(result != CURLM_OK)
    {
        std::cerr << "Failed to add easy handle to multi handle." << std::endl;
        curl_slist_free_all(header_list);
        curl_easy_cleanup(curlHandle);
        if(callback)
            callback(false, 0, "Failed to add easy handle to multi handle.");
        return;
    }

    // Create a thread to handle the async request
    std::thread worker(&HttpPostClientAsync::performAsync,
                       this,
                       curlHandle,
                       multi_handle,
                       std::ref(responseBuffer),
                       header_list,
                       callback);

    worker.detach(); // Detach the thread to let it run independently
}

size_t HttpPostClientAsync::writeCallback(void* contents,
                                          size_t size,
                                          size_t nmemb,
                                          std::string* s)
{
    size_t newLength = size * nmemb;
    try
    {
        s->append((char*) contents, newLength);
    }
    catch(std::bad_alloc& e)
    {
        return 0;
    }
    return newLength;
}

void HttpPostClientAsync::performAsync(
    CURL* curlHandle,
    CURLM* multi_handle,
    std::string& responseBuffer,
    struct curl_slist* header_list,
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
                long http_code = 0;
                curl_easy_getinfo(
                    curlHandle, CURLINFO_RESPONSE_CODE, &http_code);

                if(callback)
                    callback(true, http_code, responseBuffer); // Pass response
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

    // Clean up the easy handle and header list after completion
    curl_multi_remove_handle(multi_handle, curlHandle);
    curl_easy_cleanup(curlHandle);
    if(header_list)
        curl_slist_free_all(header_list); // Free the header list
}

void HttpPostClientAsync::cleanupCurl()
{
    if(multi_handle)
    {
        curl_multi_cleanup(multi_handle);
        multi_handle = nullptr;
    }
}

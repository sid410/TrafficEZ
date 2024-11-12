#include "HttpClient.h"

HttpClient::HttpClient(const std::string& url)
    : base_url(url)
{}

pplx::task<std::string>
HttpClient::postAsync(const std::string& endpoint,
                      const std::string& payload,
                      const std::map<std::string, std::string>& headers)
{
    http_client client(base_url);
    uri_builder builder(base_url);
    builder.append_path(endpoint);
    uri request_uri = builder.to_uri();

    http_request request(methods::POST);
    for(const auto& header : headers)
    {
        request.headers().add(header.first, header.second);
    }
    request.set_body(payload);
    request.set_request_uri(request_uri);

    // return client.request(request).then([](http_response response) {
    //     if(response.status_code() == status_codes::OK)
    //     {
    //         return response.extract_string();
    //     }
    //     else
    //     {
    //         std::cerr << "Error: " << response.status_code() << " - "
    //                   << response.reason_phrase() << std::endl;
    //         return pplx::task_from_result(std::string());
    //     }
    // });

    return client.request(request)
        .then([](http_response response) {
            if(response.status_code() == status_codes::OK)
            {
                return response.extract_string();
            }
            else
            {
                std::cerr << "Error: " << response.status_code() << " - "
                          << response.reason_phrase() << std::endl;
                return pplx::task_from_result(std::string());
            }
        })
        .then([](pplx::task<std::string> resultTask) {
            try
            {
                return resultTask.get(); // Retrieve the response or rethrow
            }
            catch(const std::exception& e)
            {
                std::cerr << "Request failed with exception: " << e.what()
                          << std::endl;
                return std::string(); // Return empty string on error
            }
        });
}

pplx::task<std::string>
HttpClient::getAsync(const std::string& endpoint,
                     const std::map<std::string, std::string>& headers)
{

    http_client client(base_url);
    uri_builder builder(base_url);
    builder.append_path(endpoint);
    uri request_uri = builder.to_uri();

    http_request request(methods::GET);
    for(const auto& header : headers)
    {
        request.headers().add(header.first, header.second);
    }
    request.set_request_uri(request_uri);

    return client.request(request).then([](http_response response) {
        if(response.status_code() == status_codes::OK)
        {
            return response.extract_string();
        }
        else
        {
            std::cerr << "Error: " << response.status_code() << " - "
                      << response.reason_phrase() << std::endl;
            return pplx::task_from_result(std::string());
        }
    });
}

// std::future<std::string>
// HttpClient::postAsync(const std::string& endpoint,
//                       const std::string& payload,
//                       const std::string& contentType = "application/json",
//                       const std::map<std::string, std::string>& headers = {})
// {
//     std::string url = this->base_url + endpoint;

//     return std::async(
//         std::launch::async, [this, url, headers, payload, contentType]() {
//             httplib::Headers h;
//             for(const auto& header : headers)
//             {
//                 h.emplace(header.first, header.second);
//             }
//             auto res = client.Post(url, h, payload, contentType.c_str());
//             if(res->status != 200)
//             {
//                 std::cerr << "Error: " << res->reason << std::endl;
//                 return std::string();
//             }
//             return res->body;
//         });
// }

// std::future<std::string>
// HttpClient::getAsync(const std::string& endpoint,
//                      const std::string& contentType = "application/json",
//                      const std::map<std::string, std::string>& headers = {})
// {
//     std::string url = this->base_url + endpoint;

//     return std::async(std::launch::async, [this, url, headers]() {
//         httplib::Headers h;
//         for(const auto& header : headers)
//         {
//             h.emplace(header.first, header.second);
//         }
//         auto res = client.Get(url.c_str(), h);
//         if(res->status != 200)
//         {
//             std::cerr << "Error: " << res->reason << std::endl;
//             return std::string();
//         }
//         return res->body;
//     });
// }

// // GET request function
// std::string getRequest(const std::string& url,
//                        const std::string& endpoint,
//                        const std::map<std::string, std::string>& headers = {})
// {
//     httplib::Client client(url);

//     // Add custom headers
//     httplib::Headers httplibHeaders;
//     for(const auto& header : headers)
//     {
//         httplibHeaders.insert(header);
//     }

//     auto res = client.Get(endpoint.c_str(), httplibHeaders);

//     if(res && res->status == 200)
//     {
//         return res->body;
//     }
//     else
//     {
//         return "GET request failed with status: " +
//                std::to_string(res ? res->status : -1);
//     }
// }

// // PUT request function
// std::string putRequest(const std::string& url,
//                        const std::string& endpoint,
//                        const std::string& data,
//                        const std::string& contentType = "application/json",
//                        const std::map<std::string, std::string>& headers = {})
// {
//     httplib::Client client(url);

//     // Add custom headers
//     httplib::Headers httplibHeaders;
//     for(const auto& header : headers)
//     {
//         httplibHeaders.insert(header);
//     }

//     auto res =
//         client.Put(endpoint.c_str(), httplibHeaders, data, contentType.c_str());

//     if(res && res->status == 200)
//     {
//         return res->body;
//     }
//     else
//     {
//         return "PUT request failed with status: " +
//                std::to_string(res ? res->status : -1);
//     }
// }

// // DELETE request function
// std::string
// deleteRequest(const std::string& url,
//               const std::string& endpoint,
//               const std::map<std::string, std::string>& headers = {})
// {
//     httplib::Client client(url);

//     // Add custom headers
//     httplib::Headers httplibHeaders;
//     for(const auto& header : headers)
//     {
//         httplibHeaders.insert(header);
//     }

//     auto res = client.Delete(endpoint.c_str(), httplibHeaders);

//     if(res && res->status == 200)
//     {
//         return res->body;
//     }
//     else
//     {
//         return "DELETE request failed with status: " +
//                std::to_string(res ? res->status : -1);
//     }
// }

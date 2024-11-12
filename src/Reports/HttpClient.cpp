#include "HttpClient.h"

HttpClient::HttpClient(const std::string& url)
    : base_url(url)
{}

pplx::task<std::string>
HttpClient::sendRequestAsync(const std::string& endpoint,
                             const std::string& payload,
                             const std::map<std::string, std::string>& headers,
                             const method& httpMethod)
{
    http_client client(base_url);
    uri_builder builder(endpoint);
    uri request_uri = builder.to_uri();

    http_request request(httpMethod);
    for(const auto& header : headers)
    {
        request.headers().add(header.first, header.second);
    }
    if(!payload.empty() &&
       (httpMethod == methods::POST || httpMethod == methods::PUT ||
        httpMethod == methods::PATCH))
    {
        request.set_body(payload);
    }
    request.set_request_uri(request_uri);

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
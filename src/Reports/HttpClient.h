#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <cpprest/http_client.h>
#include <cpprest/uri_builder.h>
#include <iostream>
#include <map>
#include <pplx/pplxtasks.h>
#include <string>

using namespace web;
using namespace web::http;
using namespace web::http::client;

class HttpClient
{
public:
    HttpClient(const std::string& url);

    // GET request
    pplx::task<std::string>
    getAsync(const std::string& endpoint,
             const std::map<std::string, std::string>& headers = {})
    {
        return sendRequestAsync(endpoint, "", headers, methods::GET);
    }

    // POST request
    pplx::task<std::string>
    postAsync(const std::string& endpoint,
              const std::string& payload,
              const std::map<std::string, std::string>& headers = {})
    {
        return sendRequestAsync(endpoint, payload, headers, methods::POST);
    }

    // PUT request
    pplx::task<std::string>
    putAsync(const std::string& endpoint,
             const std::string& payload,
             const std::map<std::string, std::string>& headers = {})
    {
        return sendRequestAsync(endpoint, payload, headers, methods::PUT);
    }

    // PATCH request
    pplx::task<std::string>
    patchAsync(const std::string& endpoint,
               const std::string& payload,
               const std::map<std::string, std::string>& headers = {})
    {
        return sendRequestAsync(endpoint, payload, headers, methods::PATCH);
    }

    // DELETE request
    pplx::task<std::string>
    deleteAsync(const std::string& endpoint,
                const std::map<std::string, std::string>& headers = {})
    {
        return sendRequestAsync(endpoint, "", headers, methods::DEL);
    }

private:
    std::string base_url;

    pplx::task<std::string>
    sendRequestAsync(const std::string& endpoint,
                     const std::string& payload,
                     const std::map<std::string, std::string>& headers,
                     const method& httpMethod);
};

#endif
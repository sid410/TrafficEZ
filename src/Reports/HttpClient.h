#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>
#include <future>
#include <string>

using namespace web::http;
using namespace web::http::client;

class HttpClient
{
public:
    HttpClient(const std::string& url);

    pplx::task<std::string>
    postAsync(const std::string& endpoint,
              const std::string& payload,
              const std::map<std::string, std::string>& headers);

    pplx::task<std::string>
    getAsync(const std::string& endpoint,
             const std::map<std::string, std::string>& headers);

private:
    std::string base_url;
};

#endif
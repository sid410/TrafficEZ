#include "Reports.h"

Reports::Reports(std::string httpUrl,
                 std::string tSecretKey,
                 std::string junctionId,
                 std::string junctionName,
                 bool verboseMode)
    : url(httpUrl)
    , tSecretKey(tSecretKey)
    , junctionId(junctionId)
    , junctionName(junctionName)
    , verbose(verboseMode)
{
    headers = {{"accept", "text/plain"},
               {"Content-Type", "application/json; charset=utf-8"},
               {"TSecretKey", tSecretKey}};
}

void Reports::sendJunctionReport(std::string& data)
{
    endpoint = "/Junction/Report";

    std::cout << "Sending density and phase time data to server...\n";

    HttpClient client(url);
    client.postAsync(endpoint, data, headers)
        .then([](const std::string& response) {
            if(!response.empty())
                std::cout << "Response: " << response << std::endl;
            else
                std::cout << "Failed to receive a valid response." << std::endl;
        });
}

void Reports::sendJunctionStatus()
{
    endpoint = "/Junction/Status";

    float warning = 0;
    float error = 0;
    float healthCheck = 100 - (warning + error);

    nlohmann::json status;
    status["junctionId"] = junctionId;
    status["name"] = junctionName;
    status["healthCheck"] = healthCheck;
    status["warning"] = warning;
    status["error"] = error;

    if(verbose)
    {
        std::cout << status.dump(2) << std::endl;
    }
    std::cout << "Sending junction status to server...\n";

    HttpClient client(url);
    client.postAsync(endpoint, status.dump(), headers)
        .then([](const std::string& response) {
            if(!response.empty())
                std::cout << "Response: " << response << std::endl;
            else
                std::cout << "Failed to receive a valid response." << std::endl;
        });
}

Reports::~Reports()
{
    std::cout << "Exiting...\n";
}
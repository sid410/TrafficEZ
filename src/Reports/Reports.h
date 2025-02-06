#ifndef REPORTS_H
#define REPORTS_H

#include "HttpClient.h"
#include <future>
#include <json.hpp>

class Reports
{
public:
    static Reports& getInstance(const std::string& httpUrl = "",
                                const std::string& tSecretKey = "",
                                const int& junctionId = 1,
                                const std::string junctionName = "",
                                bool verboseMode = false)
    {
        static Reports instance(
            httpUrl, tSecretKey, junctionId, junctionName, verboseMode);
        return instance;
    }

    Reports(const Reports&) = delete;
    Reports& operator=(const Reports&) = delete;

    void sendPhaseReport(std::string& data);
    void sendJunctionStatus();

private:
    Reports(const std::string httpUrl,
            const std::string tSecretKey,
            const int junctionId,
            const std::string junctionName,
            bool verboseMode);
    ~Reports();

    bool verbose;
    int junctionId;
    std::string junctionName;

    std::string url;
    std::string tSecretKey;
    std::map<std::string, std::string> headers;

    std::string endpoint;
    std::string contentType;
    std::string response;
};

#endif
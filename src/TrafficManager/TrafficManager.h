#ifndef TRAFFIC_MANAGER_H
#define TRAFFIC_MANAGER_H

#include <string>

class TrafficManager
{
public:
    TrafficManager(const std::string& configFile,
                   bool debug,
                   bool calib,
                   bool verbose);

    void start();

private:
    std::string configFile;
    bool debugMode;
    bool calibMode;
    bool verbose;
};

#endif

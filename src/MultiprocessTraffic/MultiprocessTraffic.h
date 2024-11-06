#ifndef MULTIPROCESS_TRAFFIC_H
#define MULTIPROCESS_TRAFFIC_H

#include "ChildProcess.h"
#include "ParentProcess.h"
#include "PhaseMessageType.h"
#include "Pipe.h"
#include <yaml-cpp/yaml.h>

#include <queue>
#include <sys/types.h>
#include <vector>

class MultiprocessTraffic
{
public:
    MultiprocessTraffic(const std::string& configFile,
                        bool debug = false,
                        bool verbose = false);

    void start();
    void parentProcessThread();
    void calibrate();

private:
    static std::queue<std::string> commandQueue;
    static std::mutex queueMutex;
    static int forkCount;
    static const int maxForkCount = 5;

    std::string configFile;
    int junctionId;
    std::string junctionName;
    bool debug;
    bool verbose;

    int numChildren;
    int numVehicle;
    int numPedestrian;
    int respawnRetries = 5;

    float densityMultiplierGreenPhase;
    float densityMultiplierRedPhase;
    float densityMin;
    float densityMax;
    int minPhaseDurationMs;
    int minPedestrianDurationMs;

    std::vector<pid_t> childPids;
    std::vector<Pipe> pipesParentToChild;
    std::vector<Pipe> pipesChildToParent;

    std::vector<std::vector<PhaseMessageType>> phases;
    std::vector<int> phaseDurations;

    std::vector<std::string> streamConfigs;
    std::vector<std::string> streamLinks;
    std::string relayUrl;
    std::string relayUsername;
    std::string relayPassword;

    static void handleSignal(int signal);
    static MultiprocessTraffic* instance;

    void createPipes();
    void forkChildren();

    void loadJunctionConfig();
    void loadPhases(const YAML::Node& config);
    void loadPhaseDurations(const YAML::Node& config);
    void loadDensitySettings(const YAML::Node& config);
    void loadStreamInfo(const YAML::Node& config);
    void loadRelayInfo(const YAML::Node& config);
    void loadJunctionInfo(const YAML::Node& config);

    void setVehicleAndPedestrianCount();
};

#endif

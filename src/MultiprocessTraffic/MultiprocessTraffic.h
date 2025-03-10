#ifndef MULTIPROCESS_TRAFFIC_H
#define MULTIPROCESS_TRAFFIC_H

#include "ChildProcess.h"
#include "ParentProcess.h"
#include "PhaseMessageType.h"
#include "Pipe.h"
#include <yaml-cpp/yaml.h>

#include <mutex>
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
    void calibrate();

private:
    std::string configFile;

    bool debug;
    bool verbose;

    int numChildren;
    int numVehicle;
    int numPedestrian;

    float densityMultiplierGreenPhase;
    float densityMultiplierRedPhase;
    float densityMin;
    float densityMax;
    int minPhaseDurationMs;
    int minPedestrianDurationMs;
    int standbyDuration;

    std::vector<pid_t> childPids;
    std::vector<Pipe> pipesParentToChild;
    std::vector<Pipe> pipesChildToParent;

    std::vector<std::vector<PhaseMessageType>> phases;
    std::vector<int> phaseDurations;
    std::vector<int> yellowChannels;

    std::vector<std::string> streamConfigs;
    std::vector<std::string> streamLinks;
    int subLocationId;
    int junctionId;
    std::string junctionName;
    std::string relayUrl;
    std::string relayUsername;
    std::string relayPassword;
    std::string httpUrl;
    std::string tSecretKey;

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
    void loadHttpInfo(const YAML::Node& config);

    void setVehicleAndPedestrianCount();
    int calculateTotalChannels(const std::vector<std::string>& childrenPhases);
    void assignChannels(const std::vector<std::string>& childrenPhases,
                        std::vector<int>& yellowChannels);
    void setYellowChannels(const YAML::Node& config);
};

#endif

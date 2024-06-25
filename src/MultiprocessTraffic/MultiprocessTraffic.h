#ifndef MULTIPROCESS_TRAFFIC_H
#define MULTIPROCESS_TRAFFIC_H

#include "ChildProcess.h"
#include "ParentProcess.h"
#include "PhaseMessageType.h"
#include "Pipe.h"
#include <yaml-cpp/yaml.h>

#include <sys/types.h>
#include <vector>

class MultiprocessTraffic
{
public:
    MultiprocessTraffic(const std::string& configFile,
                        bool debug = false,
                        bool verbose = false);

    void start();

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

    std::vector<pid_t> childPids;
    std::vector<Pipe> pipesParentToChild;
    std::vector<Pipe> pipesChildToParent;

    std::vector<std::vector<PhaseMessageType>> phases;
    std::vector<int> phaseDurations;

    std::vector<std::string> streamConfigs;
    std::vector<std::string> streamLinks;

    void createPipes();
    void forkChildren();

    void loadJunctionConfig();
    void loadPhases(const YAML::Node& config);
    void loadPhaseDurations(const YAML::Node& config);
    void loadDensitySettings(const YAML::Node& config);
    void loadStreamInfo(const YAML::Node& config);
    void setVehicleAndPedestrianCount();
};

#endif

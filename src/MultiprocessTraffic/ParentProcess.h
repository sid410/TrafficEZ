#ifndef PARENT_PROCESS_H
#define PARENT_PROCESS_H

#include "MultiprocessTraffic.h"
#include "PhaseMessageType.h"
#include "Pipe.h"
#include "Reports.h"
#include "TelnetRelayController.h"
#include <json.hpp>
#include <sstream>
#include <vector>

class ParentProcess
{
public:
    ParentProcess(int numVehicle,
                  int numPedestrian,
                  std::vector<Pipe>& pipesParentToChild,
                  std::vector<Pipe>& pipesChildToParent,
                  std::vector<std::vector<PhaseMessageType>>& phases,
                  std::vector<int>& phaseDurations,
                  bool verbose = false,
                  float densityMultiplierGreenPhase = 10.0f,
                  float densityMultiplierRedPhase = 0.3f,
                  float densityMin = 0.0f,
                  float densityMax = 50.0f,
                  int minPhaseDurationMs = 5000,
                  int minPedestrianDurationMs = 25000,
                  std::string relayUrl = "192.168.1.5",
                  int subLocationId = 1,
                  int junctionId = 1,
                  std::string junctionName = "");
    void run();

private:
    static constexpr int BUFFER_SIZE = 128;

    TelnetRelayController& telnetRelay = TelnetRelayController::getInstance();
    Reports& report = Reports::getInstance();

    bool verbose;
    int cycle;
    int subLocationId;
    int junctionId;
    std::string junctionName;
    std::string relayUrl;

    float densityMultiplierGreenPhase;
    float densityMultiplierRedPhase;
    float densityMin;
    float densityMax;

    int minPhaseDurationMs;
    int minPedestrianDurationMs;
    int fullCycleDurationMs;

    int numVehicle;
    int numPedestrian;
    int numChildren;

    std::vector<Pipe>& pipesParentToChild;
    std::vector<Pipe>& pipesChildToParent;

    std::vector<std::vector<PhaseMessageType>>& phases;
    std::vector<int>& phaseDurations;
    std::vector<int> originalPhaseDurations;
    std::vector<float> phaseRatio;

    void sendPhaseMessagesToChildren(int phaseIndex);

    bool receivePrevDataFromChildren(
        int phaseIndex,
        std::vector<std::vector<float>>& phaseDensities,
        std::vector<std::vector<float>>& phaseSpeeds,
        std::vector<std::vector<std::unordered_map<std::string, int>>>&
            phaseVehicles);

    bool readDataFromChild(int childIndex,
                           float& density,
                           float& speed,
                           std::unordered_map<std::string, int>& vehicles);

    void processDensityByPhaseType(PhaseMessageType phaseType, float& density);

    void handlePhaseTimer(int phaseIndex);

    void transitionToNextPhase(
        int& phaseIndex,
        std::vector<std::vector<float>>& phaseDensities,
        std::vector<std::vector<float>>& phaseSpeeds,
        std::vector<std::vector<std::unordered_map<std::string, int>>>&
            phaseVehicles);

    void
    setDefaultPhaseDensities(std::vector<std::vector<float>>& phaseDensities);

    void updatePhaseDurations(
        const std::vector<std::vector<float>>& phaseDensities,
        const std::vector<std::vector<float>>& phaseSpeeds,
        std::vector<std::vector<std::unordered_map<std::string, int>>>&
            phaseVehicles);

    void closeUnusedPipes();
};
#endif

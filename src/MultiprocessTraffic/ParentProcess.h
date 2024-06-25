#ifndef PARENT_PROCESS_H
#define PARENT_PROCESS_H

#include "PhaseMessageType.h"
#include "Pipe.h"
#include <vector>

class ParentProcess
{
public:
    ParentProcess(int numVehicle,
                  int numPedestrian,
                  std::vector<Pipe>& pipesParentToChild,
                  std::vector<Pipe>& pipesChildToParent,
                  std::vector<std::vector<const char*>>& phases,
                  std::vector<int>& phaseDurations,
                  bool verbose = false,
                  float densityMultiplierGreenPhase = 10.0f,
                  float densityMultiplierRedPhase = 0.3f,
                  float densityMin = 0.0f,
                  float densityMax = 50.0f,
                  int minPhaseDurationMs = 5000,
                  int minPedestrianDurationMs = 25000);
    void run();

private:
    static constexpr int BUFFER_SIZE = 128;

    bool verbose;

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

    std::vector<std::vector<const char*>>& phases;
    std::vector<int>& phaseDurations;
    std::vector<int> originalPhaseDurations;
    std::vector<float> phaseRatio;

    void sendPhaseMessagesToChildren(int phaseIndex);

    bool receivePrevDensitiesFromChildren(
        int previousPhaseIndex,
        std::vector<std::vector<float>>& phaseDensities);
    bool readDensityFromChild(int childIndex, float& density);
    void processDensityByPhaseType(PhaseMessageType phaseType, float& density);

    void handlePhaseTimer(int phaseIndex);
    void transitionToNextPhase(int& phaseIndex,
                               std::vector<std::vector<float>>& phaseDensities);

    void
    setDefaultPhaseDensities(std::vector<std::vector<float>>& phaseDensities);
    void
    updatePhaseDurations(const std::vector<std::vector<float>>& phaseDensities);

    void closeUnusedPipes();
};

#endif

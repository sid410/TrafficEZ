#ifndef PARENT_PROCESS_H
#define PARENT_PROCESS_H

#include "Pipe.h"

#include <vector>

class ParentProcess
{
public:
    ParentProcess(int numChildren,
                  std::vector<Pipe>& pipesParentToChild,
                  std::vector<Pipe>& pipesChildToParent,
                  std::vector<std::vector<const char*>>& phases,
                  std::vector<int>& phaseDurations,
                  bool verbose = false,
                  float densityMultiplierGreenPhase = 10.0f,
                  float densityMultiplierRedPhase = 0.3f,
                  float densityMin = 0.0f,
                  float densityMax = 50.0f,
                  int minPhaseDurationMs = 5000);
    void run();

private:
    static constexpr int BUFFER_SIZE = 128;

    bool verbose;

    float densityMultiplierGreenPhase;
    float densityMultiplierRedPhase;
    float densityMin;
    float densityMax;

    int minPhaseDurationMs;
    int fullCycleDurationMs;

    int numChildren;
    std::vector<Pipe>& pipesParentToChild;
    std::vector<Pipe>& pipesChildToParent;

    std::vector<std::vector<const char*>>& phases;
    std::vector<int>& phaseDurations;
    std::vector<int> originalPhaseDurations;
    std::vector<float> phaseRatio;

    void sendPhaseMessagesToChildren(int phaseIndex);
    bool receiveDensitiesFromChildren(
        int previousPhaseIndex,
        std::vector<std::vector<float>>& phaseDensities);

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

#include "ParentProcess.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <limits>
#include <numeric>
#include <sys/wait.h>
#include <unistd.h>

ParentProcess::ParentProcess(int numVehicle,
                             int numPedestrian,
                             std::vector<Pipe>& pipesParentToChild,
                             std::vector<Pipe>& pipesChildToParent,
                             std::vector<std::vector<const char*>>& phases,
                             std::vector<int>& phaseDurations,
                             bool verbose,
                             float densityMultiplierGreenPhase,
                             float densityMultiplierRedPhase,
                             float densityMin,
                             float densityMax,
                             int minPhaseDurationMs,
                             int minPedestrianDurationMs)
    : numVehicle(numVehicle)
    , numPedestrian(numPedestrian)
    , pipesParentToChild(pipesParentToChild)
    , pipesChildToParent(pipesChildToParent)
    , phases(phases)
    , phaseDurations(phaseDurations)
    , verbose(verbose)
    , densityMultiplierGreenPhase(densityMultiplierGreenPhase)
    , densityMultiplierRedPhase(densityMultiplierRedPhase)
    , densityMin(densityMin)
    , densityMax(densityMax)
    , minPhaseDurationMs(minPhaseDurationMs)
    , minPedestrianDurationMs(minPedestrianDurationMs)
{
    numChildren = numVehicle + numPedestrian;

    originalPhaseDurations = phaseDurations;

    fullCycleDurationMs = std::accumulate(
        originalPhaseDurations.begin(), originalPhaseDurations.end(), 0.0f);

    phaseRatio.clear();
    for(const auto& duration : originalPhaseDurations)
    {
        float ratio = static_cast<float>(duration) / fullCycleDurationMs;
        phaseRatio.push_back(ratio);
    }

    closeUnusedPipes();
}

void ParentProcess::run()
{
    int phaseIndex = 0;

    std::vector<std::vector<float>> phaseDensities(
        phases.size(), std::vector<float>(numChildren, 0.0));

    while(true)
    {
        if(verbose)
        {
            std::cout << "==================== Phase Cycle " << phaseIndex
                      << " ======================================\n";
        }

        sendPhaseMessagesToChildren(phaseIndex);

        if(!receivePrevDensitiesFromChildren(phaseIndex, phaseDensities))
        {
            setDefaultPhaseDensities(phaseDensities);
        }

        // TODO: place here sendSerialMessageToRelay(phases, phaseindex, numChildren, verbose)

        handlePhaseTimer(phaseIndex);
        transitionToNextPhase(phaseIndex, phaseDensities);
    }

    for(int i = 0; i < numChildren; ++i)
    {
        wait(nullptr); // Wait for child processes to finish
    }
}

void ParentProcess::sendPhaseMessagesToChildren(int phaseIndex)
{
    for(int i = 0; i < numChildren; ++i)
    {
        if(verbose)
        {
            std::cout << "Parent: Sending phase message to child " << i << ": "
                      << phases[phaseIndex][i] << "\n";
        }

        if(write(pipesParentToChild[i].fds[1],
                 phases[phaseIndex][i],
                 strlen(phases[phaseIndex][i]) + 1) == -1)
        {
            std::cerr << "Parent: Failed to write to pipe: " << strerror(errno)
                      << "\n";
            break;
        }
    }
}

bool ParentProcess::receivePrevDensitiesFromChildren(
    int phaseIndex, std::vector<std::vector<float>>& phaseDensities)
{
    char buffer[BUFFER_SIZE];

    int previousPhaseIndex =
        (phaseIndex == 0) ? phases.size() - 1 : phaseIndex - 1;

    for(int i = 0; i < numChildren; ++i)
    {
        int bytesRead =
            read(pipesChildToParent[i].fds[0], buffer, sizeof(buffer) - 1);
        if(bytesRead <= 0)
        {
            std::cerr << "Parent: Failed to read from pipe or EOF reached: "
                      << strerror(errno) << "\n";
            return false;
        }
        buffer[bytesRead] = '\0'; // Ensure null termination

        // Convert buffer to float
        float density = std::stof(buffer);

        // Check if density is NaN or negative NaN
        if(std::isnan(density) ||
           (std::isnan(density) && std::signbit(density)))
        {
            std::cerr << "Parent: Detected NaN or negative NaN in traffic "
                         "density from child "
                      << i << "\n";

            return false;
        }

        // If previous phase was green, multiply density by the multiplier
        if(strcmp(phases[previousPhaseIndex][i], "GREEN_PHASE") == 0)
        {
            density *= densityMultiplierGreenPhase;
        }

        // If previous phase was red, give way to other phase cycle
        if(strcmp(phases[previousPhaseIndex][i], "RED_PHASE") == 0)
        {
            density = (densityMax - density) * densityMultiplierRedPhase;
        }

        // previous is green pedestrian, so ignore by setting 0
        // if previous was red, the received message should be
        // greater than 0 if there are waiting pedestrians
        if(strcmp(phases[previousPhaseIndex][i], "RED_PED") == 0)
        {
            density = 0;
        }

        density = std::clamp(density, densityMin, densityMax);

        if(verbose)
        {
            std::cout << "Previous Traffic Density from child " << i << ": "
                      << density << "\n";
        }

        // Store density to the previous phase
        phaseDensities[previousPhaseIndex][i] = density;
    }

    return true;
}

void ParentProcess::handlePhaseTimer(int phaseIndex)
{
    int remainingTime = phaseDurations[phaseIndex] / 1000;

    while(remainingTime > 0)
    {
        std::cout << "\rRemaining time for phase " << phaseIndex << ": "
                  << remainingTime << " seconds.";
        std::cout.flush();
        sleep(1);
        --remainingTime;
    }
    std::cout << std::endl;
}

void ParentProcess::transitionToNextPhase(
    int& phaseIndex, std::vector<std::vector<float>>& phaseDensities)
{
    phaseIndex = (phaseIndex + 1) % phases.size();

    if(phaseIndex == 0)
    {
        updatePhaseDurations(phaseDensities);
    }
}

void ParentProcess::setDefaultPhaseDensities(
    std::vector<std::vector<float>>& phaseDensities)
{
    if(phaseDensities.size() != phaseRatio.size())
    {
        std::cerr << "Size of phase density and ratio do not match!\n";
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < phaseRatio.size(); ++i)
    {
        phaseDensities[i] = std::vector<float>(numChildren, phaseRatio[i]);
    }

    std::cout << "Parent: Phase densities set to default values.\n";
}

void ParentProcess::updatePhaseDurations(
    const std::vector<std::vector<float>>& phaseDensities)
{
    float totalDensity = 0.0;
    std::vector<float> phaseTotals(phases.size(), 0.0);
    std::vector<float> pedestrianTotals(phases.size(), 0.0);

    if(verbose)
    {
        std::cout << "----- Density Distribution ------------\n";
    }

    for(int phase = 0; phase < phases.size(); ++phase)
    {
        for(int child = 0; child < numVehicle; ++child)
        {
            phaseTotals[phase] += phaseDensities[phase][child];

            if(verbose)
            {
                std::cout << "Phase " << phase << " - child " << child
                          << " density: " << phaseDensities[phase][child]
                          << "\n";
            }
        }
        totalDensity += phaseTotals[phase];

        for(int child = numVehicle; child < numChildren; ++child)
        {
            pedestrianTotals[phase] += phaseDensities[phase][child];

            if(verbose)
            {
                std::cout << "Phase " << phase << " - child " << child
                          << " pedestrian: " << phaseDensities[phase][child]
                          << "\n";
            }
        }
    }

    std::cout << "----------------------------------------------------------\n";

    // Update phase durations based on the density ratios
    bool validDurations = true;
    for(int phase = 0; phase < phases.size(); ++phase)
    {
        phaseDurations[phase] = static_cast<int>(
            (totalDensity == 0.0 ? 1.0 : phaseTotals[phase] / totalDensity) *
            fullCycleDurationMs);

        // so we always have enough time for yellow
        if(phaseDurations[phase] < minPhaseDurationMs)
        {
            phaseDurations[phase] = minPhaseDurationMs;
        }

        if(phaseDurations[phase] > fullCycleDurationMs)
        {
            validDurations = false;
            break;
        }

        // if there is waiting pedestrian, prioritize them
        if(pedestrianTotals[phase] > 0 &&
           phaseDurations[phase] < minPedestrianDurationMs)
        {
            phaseDurations[phase] = minPedestrianDurationMs;
        }

        std::cout << "Phase " << phase
                  << " allocated time: " << phaseDurations[phase] / 1000.0
                  << " seconds.\n";
    }

    std::cout << "----------------------------------------------------------\n";

    if(!validDurations)
    {
        phaseDurations = originalPhaseDurations;
        std::cerr << "Parent: Phase durations set to original values due to "
                     "invalid duration.\n";
    }
}

void ParentProcess::closeUnusedPipes()
{
    for(int i = 0; i < numChildren; ++i)
    {
        close(pipesParentToChild[i].fds[0]);
        close(pipesChildToParent[i].fds[1]);
    }
}
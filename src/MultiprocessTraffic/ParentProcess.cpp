#include "ParentProcess.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <limits>
#include <numeric>
#include <sys/wait.h>
#include <unistd.h>

ParentProcess::ParentProcess(int numChildren,
                             std::vector<Pipe>& pipesParentToChild,
                             std::vector<Pipe>& pipesChildToParent,
                             std::vector<std::vector<const char*>>& phases,
                             std::vector<int>& phaseDurations,
                             float densityMultiplierGreenPhase,
                             float densityMultiplierRedPhase,
                             float densityMin,
                             float densityMax,
                             int minPhaseDurationMs)
    : numChildren(numChildren)
    , pipesParentToChild(pipesParentToChild)
    , pipesChildToParent(pipesChildToParent)
    , phases(phases)
    , phaseDurations(phaseDurations)
    , densityMultiplierGreenPhase(densityMultiplierGreenPhase)
    , densityMultiplierRedPhase(densityMultiplierRedPhase)
    , densityMin(densityMin)
    , densityMax(densityMax)
    , minPhaseDurationMs(minPhaseDurationMs)
{
    originalPhaseDurations = phaseDurations;

    fullCycleDurationMs = std::accumulate(
        originalPhaseDurations.begin(), originalPhaseDurations.end(), 0.0f);

    phaseRatio.clear();
    for(const auto& duration : originalPhaseDurations)
    {
        float ratio = static_cast<float>(duration) / fullCycleDurationMs;
        phaseRatio.push_back(ratio);
    }
}

void ParentProcess::run()
{
    closeUnusedPipes();

    char buffer[128];
    int phaseIndex = 0;
    std::vector<std::vector<float>> phaseDensities(
        phases.size(), std::vector<float>(numChildren, 0.0));

    while(true)
    {
        std::cout << "==================== Phase Cycle " << phaseIndex
                  << " ======================================\n";

        for(int i = 0; i < numChildren; ++i)
        {
            std::cout << "Parent: Sending phase message to child " << i << ": "
                      << phases[phaseIndex][i] << "\n";
            if(write(pipesParentToChild[i].fds[1],
                     phases[phaseIndex][i],
                     strlen(phases[phaseIndex][i]) + 1) == -1)
            {
                std::cerr << "Parent: Failed to write to pipe: "
                          << strerror(errno) << "\n";
                break;
            }
        }

        bool densityError = false;
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
                break;
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

                densityError = true;
                break;
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

            density = std::clamp(density, densityMin, densityMax);

            std::cout << "Previous Traffic Density from child " << i << ": "
                      << density << "\n";

            // Store density to the previous phase
            phaseDensities[previousPhaseIndex][i] = density;
        }

        if(densityError)
        {
            setDefaultPhaseDensities(phaseDensities);
        }

        // Waiting time for the current phase
        usleep(phaseDurations[phaseIndex] * 1000);

        // Move to the next phase
        phaseIndex = (phaseIndex + 1) % phases.size();

        // If completed a full cycle, update phase durations
        if(phaseIndex == 0)
        {
            updatePhaseDurations(phaseDensities);
        }
    }

    for(int i = 0; i < numChildren; ++i)
    {
        wait(nullptr); // Wait for child processes to finish
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

void ParentProcess::updatePhaseDurations(
    const std::vector<std::vector<float>>& phaseDensities)
{
    // Calculate total density for each phase
    std::vector<float> phaseTotals(phases.size(), 0.0);
    float totalDensity = 0.0;

    std::cout << "----------------------------------------------------------\n";
    for(int phase = 0; phase < phases.size(); ++phase)
    {
        for(int child = 0; child < numChildren; ++child)
        {
            phaseTotals[phase] += phaseDensities[phase][child];
            std::cout << "Phase " << phase << " - child " << child
                      << " density: " << phaseDensities[phase][child] << "\n";
        }
        totalDensity += phaseTotals[phase];
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

        std::cout << "Phase " << phase
                  << " allocated time: " << phaseDurations[phase] / 1000.0
                  << " seconds.\n";
    }

    if(!validDurations)
    {
        phaseDurations = originalPhaseDurations;
        std::cerr << "Parent: Phase durations set to original values due to "
                     "invalid duration.\n";
    }

    std::cout << "----------------------------------------------------------\n";
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
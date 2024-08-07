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
                             std::vector<std::vector<PhaseMessageType>>& phases,
                             std::vector<int>& phaseDurations,
                             bool verbose,
                             float densityMultiplierGreenPhase,
                             float densityMultiplierRedPhase,
                             float densityMin,
                             float densityMax,
                             int minPhaseDurationMs,
                             int minPedestrianDurationMs,
                             std::string relayUrl)
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
    , relayUrl(relayUrl)
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
    relay.initialize(phases, relayUrl, verbose);

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

        relay.setPhaseCycle(phaseIndex);
        relay.executePhase();

        sendPhaseMessagesToChildren(phaseIndex);

        if(!receivePrevDensitiesFromChildren(phaseIndex, phaseDensities))
        {
            setDefaultPhaseDensities(phaseDensities);
        }

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

        std::string phaseString;
        switch(phases[phaseIndex][i])
        {
        case GREEN_PHASE:
            phaseString = "GREEN_PHASE";
            break;
        case RED_PHASE:
            phaseString = "RED_PHASE";
            break;
        case GREEN_PED:
            phaseString = "GREEN_PED";
            break;
        case RED_PED:
            phaseString = "RED_PED";
            break;
        default:
            phaseString = "UNKNOWN";
            break;
        }

        if(write(pipesParentToChild[i].fds[1],
                 phaseString.c_str(),
                 phaseString.size() + 1) == -1)
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
        float density;
        if(!readDensityFromChild(i, density))
        {
            return false;
        }

        PhaseMessageType previousPhaseType = phases[previousPhaseIndex][i];
        processDensityByPhaseType(previousPhaseType, density);
        density = std::clamp(density, densityMin, densityMax);

        if(verbose)
        {
            std::cout << "Previous child " << i << " data: " << density << "\n";
        }

        phaseDensities[previousPhaseIndex][i] = density;
    }

    return true;
}

bool ParentProcess::readDensityFromChild(int childIndex, float& density)
{
    char buffer[BUFFER_SIZE];
    int bytesRead =
        read(pipesChildToParent[childIndex].fds[0], buffer, sizeof(buffer) - 1);
    if(bytesRead <= 0)
    {
        std::cerr << "Parent: Failed to read from pipe or EOF reached: "
                  << strerror(errno) << "\n";
        return false;
    }
    buffer[bytesRead] = '\0'; // Ensure null termination

    try
    {
        density = std::stof(buffer);
    }
    catch(const std::invalid_argument&)
    {
        std::cerr << "Parent: Invalid density value received from child "
                  << childIndex << "\n";
        return false;
    }

    if(std::isnan(density) || (std::isnan(density) && std::signbit(density)))
    {
        std::cerr << "Parent: Detected NaN or negative NaN in traffic density "
                     "from child "
                  << childIndex << "\n";
        return false;
    }

    return true;
}

void ParentProcess::processDensityByPhaseType(PhaseMessageType phaseType,
                                              float& density)
{
    switch(phaseType)
    {
    case GREEN_PHASE:
        density *= densityMultiplierGreenPhase;
        break;
    case RED_PHASE:
        density = (densityMax - density) * densityMultiplierRedPhase;
        break;
    case RED_PED:
        density = 0;
        break;
    case GREEN_PED:
        // No specific handling needed
        break;
    default:
        break;
    }
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

        if(remainingTime == 5)
        {
            relay.executeTransitionPhase();
        }

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
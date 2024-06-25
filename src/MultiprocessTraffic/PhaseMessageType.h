#ifndef PHASE_MESSAGE_TYPE_H
#define PHASE_MESSAGE_TYPE_H

#include <string>
#include <unordered_map>

enum PhaseMessageType
{
    RED_PHASE,
    GREEN_PHASE,
    RED_PED,
    GREEN_PED,
    UNKNOWN
};

PhaseMessageType getPhaseMessageType(const std::string& message);

#endif

#include "PhaseMessageType.h"

PhaseMessageType getPhaseMessageType(const std::string& message)
{
    static const std::unordered_map<std::string, PhaseMessageType> messageMap =
        {{"RED_PHASE", RED_PHASE},
         {"GREEN_PHASE", GREEN_PHASE},
         {"RED_PED", RED_PED},
         {"GREEN_PED", GREEN_PED}};

    auto it = messageMap.find(message);
    if(it != messageMap.end())
    {
        return it->second;
    }
    return UNKNOWN;
}

#ifndef VEHICLE_SEGMENTATION_STRATEGY_H
#define VEHICLE_SEGMENTATION_STRATEGY_H

#include "ISegmentationStrategy.h"
#include <unordered_set>

class VehicleSegmentationStrategy : public ISegmentationStrategy
{
public:
    std::vector<YoloResults>
    filterResults(const std::vector<YoloResults>& results) override;
};

#endif

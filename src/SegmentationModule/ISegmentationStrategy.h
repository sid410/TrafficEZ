#ifndef I_SEGMENTATION_STRATEGY_H
#define I_SEGMENTATION_STRATEGY_H

#include "AutoBackendOnnx.h"
#include "YoloUtils.h"
#include <vector>

class ISegmentationStrategy
{
public:
    virtual std::vector<YoloResults>
    filterResults(const std::vector<YoloResults>& results) = 0;

    virtual ~ISegmentationStrategy() = default;
};

#endif

#ifndef PERSON_SEGMENTATION_STRATEGY_H
#define PERSON_SEGMENTATION_STRATEGY_H

#include "ISegmentationStrategy.h"
#include <iostream>

class PersonSegmentationStrategy : public ISegmentationStrategy
{
public:
    std::vector<YoloResults>
    filterResults(const std::vector<YoloResults>& results) override;
};

#endif

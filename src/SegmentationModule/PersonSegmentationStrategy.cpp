#include "PersonSegmentationStrategy.h"

std::vector<YoloResults> PersonSegmentationStrategy::filterResults(
    const std::vector<YoloResults>& results)
{
    std::vector<YoloResults> filtered;

    std::copy_if(results.begin(),
                 results.end(),
                 std::back_inserter(filtered),
                 [](const YoloResults& result) {
                     return result.class_idx == 0; // Person class
                 });

    return filtered;
}

#include "VehicleSegmentationStrategy.h"

std::vector<YoloResults> VehicleSegmentationStrategy::filterResults(
    const std::vector<YoloResults>& results)
{
    std::vector<YoloResults> filtered;
    std::unordered_set<int> allowedClasses = {1, 2, 3, 5, 6, 7};

    std::copy_if(results.begin(),
                 results.end(),
                 std::back_inserter(filtered),
                 [&allowedClasses](const YoloResults& result) {
                     return allowedClasses.find(result.class_idx) !=
                            allowedClasses.end();
                 });

    return filtered;
}

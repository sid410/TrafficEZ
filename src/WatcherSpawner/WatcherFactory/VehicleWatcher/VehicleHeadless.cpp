#include "VehicleHeadless.h"
#include <iostream>

void VehicleHeadless::process(const std::string& streamName,
                              const std::string& calibName)
{
    std::cout << "Stream Name: " << streamName << std::endl;
    std::cout << "Calibration Name: " << calibName << std::endl;
    std::cerr << "No implementation yet for Vehicle Headless\n";
}

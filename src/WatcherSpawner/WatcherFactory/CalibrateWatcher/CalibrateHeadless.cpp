#include "CalibrateHeadless.h"
#include <iostream>

void CalibrateHeadless::process(const std::string& streamName,
                                const std::string& calibName) const
{
    std::cout << "Stream Name: " << streamName << std::endl;
    std::cout << "Calibration Name: " << calibName << std::endl;
    std::cerr << "No implementation yet for Calibrate Headless\n";
}

#include "CalibrateHeadless.h"
#include <iostream>

void CalibrateHeadless::initialize(const std::string& streamName,
                                   const std::string& calibName)
{
    std::cout << "Stream Name: " << streamName << std::endl;
    std::cout << "Calibration Name: " << calibName << std::endl;
    std::cerr << "There is no implementation for Calibrate Headless\n";
    exit(EXIT_FAILURE);
}

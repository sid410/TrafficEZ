#include "TrafficManager.h"
#include "cxxopts.hpp"
#include "version_config.h"
#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @brief Entry point of how the program should run.
 *
 * This is where you parse and pass the arguments,
 * and create the TrafficManager object.
 */
int main(int argc, char* argv[])
{
    cxxopts::Options options(PROJECT_NAME_VER, "----------");

    options.add_options()("d,debug",
                          "Enable debugging",
                          cxxopts::value<bool>()->default_value("false"))(
        "c,calib",
        "Enter calibration",
        cxxopts::value<bool>()->default_value("false"))(
        "v,vehicle",
        "Number of vehicle ROIs",
        cxxopts::value<int>()->default_value("3"))(
        "p,pedestrian",
        "Number of pedestrian ROIs",
        cxxopts::value<int>()->default_value("2"))("h,help", "Print usage");

    auto result = options.parse(argc, argv);

    if(result.count("help"))
    {
        std::cout << options.help();
        exit(0);
    }

    bool debug = result["debug"].as<bool>();
    bool calib = result["calib"].as<bool>();
    int numCarRoi = result["vehicle"].as<int>();
    int numPedRoi = result["pedestrian"].as<int>();

    if(debug)
    {
        std::cout << "Project version: " << PROJECT_NAME_VER << "\n";
        std::cout << "OpenCV version: " << CV_VERSION << "\n";
        std::cout << "Number of CPU cores: " << cv::getNumberOfCPUs() << "\n";
    }

    // should change whether numCarRoi + numPedRoi > numCpuCores
    TrafficManager trafficManager(numCarRoi, numPedRoi, debug, calib);
    trafficManager.start();

    return 0;
}
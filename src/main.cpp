#include "cxxopts.hpp"
#include "version_config.h"
#include <iostream>
#include <opencv2/opencv.hpp>

/**
 * @brief Entry point of how the program should run.
 *
 * This is where you parse and pass the arguments,
 * and create the TrafficManager object.
 *
 * @return Exit successfully after instantiating TrafficManager.
 */
int main(int argc, char* argv[])
{
    cxxopts::Options options(PROJECT_NAME_VER, "----------");

    options.add_options()(
        "d,debug", "Enable debugging", cxxopts::value<bool>()->default_value("false"))(
        "c,car", "Number of car ROIs", cxxopts::value<int>()->default_value("3"))(
        "p,pedestrian", "Number of pedestrian ROIs", cxxopts::value<int>()->default_value("2"))(
        "h,help", "Print usage");

    auto result = options.parse(argc, argv);

    if(result.count("help"))
    {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    bool debug = result["debug"].as<bool>();
    int numCarRoi = result["car"].as<int>();
    int numPedRoi = result["pedestrian"].as<int>();

    if(debug)
    {
        std::cout << "Project version: " << PROJECT_NAME_VER << std::endl;
        std::cout << "OpenCV version: " << CV_VERSION << std::endl;
        std::cout << "Number of CPU cores: " << cv::getNumberOfCPUs() << std::endl;
        std::cout << "Number of car_roi: " << numCarRoi << std::endl;
        std::cout << "Number of ped_roi: " << numPedRoi << std::endl;
        // do some debugging here
    }

    // Instantiate here TrafficManager, and should change whether:
    // numCarRoi + numPedRoi > numCpuCores

    return 0;
}
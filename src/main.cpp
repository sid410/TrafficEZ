#include "TrafficManager.h"
#include "cxxopts.hpp"
#include "version_config.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <utils/logging.hpp>

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
                          "Enable debugging (GUI mode)",
                          cxxopts::value<bool>()->default_value("false"))(
        "c,calib",
        "Calibration mode",
        cxxopts::value<bool>()->default_value("false"))(
        "v,verbose",
        "Verbose mode",
        cxxopts::value<bool>()->default_value("false"))(
        "t,test", "Test mode", cxxopts::value<bool>()->default_value("false"))(
        "j,jconf",
        "Junction config",
        cxxopts::value<std::string>()->default_value("junction_config.yaml"))(
        "h,help", "Print usage");

    auto result = options.parse(argc, argv);

    if(result.count("help"))
    {
        std::cout << options.help();
        exit(0);
    }

    bool debug = result["debug"].as<bool>();
    bool calib = result["calib"].as<bool>();
    bool verbose = result["verbose"].as<bool>();
    bool test = result["test"].as<bool>();
    std::string configFile = result["jconf"].as<std::string>();

    Logging::setupLogging();
    if(verbose)
    {
        BOOST_LOG_TRIVIAL(info)
            << PROJECT_NAME_VER << " | OpenCV " << CV_VERSION
            << " | CPU Cores: " << cv::getNumberOfCPUs() << "\n";
    }

    TrafficManager trafficManager(configFile, debug, calib, verbose, test);
    trafficManager.start();

    return 0;
}
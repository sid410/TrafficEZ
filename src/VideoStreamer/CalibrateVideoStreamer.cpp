#include "CalibrateVideoStreamer.h"
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

CalibrateVideoStreamer::CalibrateVideoStreamer()
    : VideoStreamer()
    , pointsSetSuccessfully(false)
{}

CalibrateVideoStreamer::~CalibrateVideoStreamer() {}

/**
 * @brief non-member function only used for setMouseCallback
 * of CalibrateVideoStreamer::initCalibrationPoints
 */
void onMouseClickCallback(int event, int x, int y, int flags, void* userdata)
{
    auto& points = *static_cast<std::vector<cv::Point2f>*>(userdata);

    // maximum of four points to set the ROI
    if(event == cv::EVENT_LBUTTONDOWN && points.size() < 4)
    {
        points.emplace_back(x, y);
        std::cout << "Point (" << x << ", " << y << ") added.\n";
    }
}

/**
 * @brief Initialize the mouse callback function to set four ROI points.
 * @param windowName the window to listen for mouse click callback.
 */
void CalibrateVideoStreamer::initCalibrationPoints(const cv::String& windowName)
{
    std::cout << "Please click on four points (x, y) for calibration.\n"
              << "Press 'r' to reset, 'p' to preview between warp/trim, "
                 "and 's' to save and exit.\n";

    cv::setMouseCallback(
        windowName, onMouseClickCallback, &mouseCalibrationPoints);
}

/**
 * @brief Clears the currently held mouseCalibrationPoints.
 */
void CalibrateVideoStreamer::resetCalibrationPoints()
{
    mouseCalibrationPoints.clear();
    std::cout << "Calibration points reset.\n";
}

/**
 * @brief Saves the four mouseCalibrationPoints and 
 * the lanes total length and width to a yaml file.
 * After saving, we can exit the calibration loop by setting
 * pointsSetSuccessfully = true.
 * @param filename the name of the yaml file to save the four ROI points.
 */
void CalibrateVideoStreamer::saveCalibrationPoints(const cv::String& filename)
{
    if(!haveSetFourPoints())
        return; // need to have four points to define the ROI quadrilateral.

    // Ask for lane dimensions
    double laneLength, laneWidth;
    std::cout << "Enter lane length: ";
    std::cin >> laneLength;
    std::cout << "Enter lane width: ";
    std::cin >> laneWidth;

    YAML::Emitter emitter;
    emitter << YAML::BeginMap;

    // Saving calibration points
    emitter << YAML::Key << "calibration_points";
    emitter << YAML::Value << YAML::BeginSeq;
    for(const auto& point : mouseCalibrationPoints)
    {
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "x" << YAML::Value << point.x;
        emitter << YAML::Key << "y" << YAML::Value << point.y;
        emitter << YAML::EndMap;
    }
    emitter << YAML::EndSeq;

    // Saving lane dimensions
    emitter << YAML::Key << "lanes_dimension";
    emitter << YAML::Value << YAML::BeginSeq;
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "length" << YAML::Value << laneLength;
    emitter << YAML::Key << "width" << YAML::Value << laneWidth;
    emitter << YAML::EndMap;
    emitter << YAML::EndSeq;

    emitter << YAML::EndMap;

    std::ofstream fout(filename);
    fout << emitter.c_str();
    fout.close();

    std::cout << "Calibration info saved to " << filename << ".\n";
    pointsSetSuccessfully = true; // exit the calibration loop.
}

/**
 * @brief Visualize the current calibration points set by mouse.
 * @param frame the frame to visualize the calibration points.
 */
void CalibrateVideoStreamer::showCalibrationPoints(cv::Mat& frame)
{
    for(const auto& point : mouseCalibrationPoints)
    {
        cv::circle(
            frame, cv::Point(point.x, point.y), 5, cv::Scalar(0, 255, 255), -1);
    } // visualize yellow dots at each point.
}

/**
 * @brief An extension of the parent class function initializePerspectiveTransform.
 * This is so we can preview the Warped/Trimmed view before saving the ROI points.
 */
void CalibrateVideoStreamer::initializePreview(
    cv::Mat& frame, TransformPerspective& perspective)
{
    roiPoints = mouseCalibrationPoints;
    readCalibSuccess = true; // this is a workaround to pass the check
    // of yaml file successfully reading the calibration points.

    initializePerspectiveTransform(frame, perspective);
}

/**
 * @brief Use this to control the while-loop logic while
 * setting the calibration points.
 * @param frame reference for the next stream frame.
 * @return true if still in calibration phase and
 * next frame is available.
 */
bool CalibrateVideoStreamer::settingCalibrationPoints(cv::Mat& frame)
{
    if(!getNextFrame(frame))
    {
        std::cerr << "Error: Unable to retrieve the next frame.\n";
        return false;
    }

    // so we can exit the calibration loop.
    return !pointsSetSuccessfully;
}

/**
 * @brief Check for having exactly four points to save.
 * @return true if mouseCalibrationPoints is exactly four points. 
 */
bool CalibrateVideoStreamer::haveSetFourPoints()
{
    if(mouseCalibrationPoints.size() != 4)
    {
        std::cout
            << "Please set exactly 4 calibration points before proceeding.\n";
        return false;
    }

    return true;
}
#include "CalibrateVideoStreamer.h"
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

CalibrateVideoStreamer::CalibrateVideoStreamer()
    : VideoStreamer()
    , pointsSetSuccessfully(false)
{}

CalibrateVideoStreamer::~CalibrateVideoStreamer() {}

void onMouseClickCallback(int event, int x, int y, int flags, void* userdata)
{
    auto& points = *static_cast<std::vector<cv::Point2f>*>(userdata);

    if(event == cv::EVENT_LBUTTONDOWN && points.size() < 4)
    {
        points.emplace_back(x, y);
        std::cout << "Point (" << x << ", " << y << ") added.\n";
    }
}

void CalibrateVideoStreamer::initCalibrationPoints(const cv::String& windowName)
{
    std::cout << "Please click on four points (x, y) for calibration.\n"
              << "Press 'r' to reset, or 's' to save and exit.\n";

    cv::setMouseCallback(
        windowName, onMouseClickCallback, &mouseCalibrationPoints);
}

void CalibrateVideoStreamer::resetCalibrationPoints()
{
    mouseCalibrationPoints.clear();
    std::cout << "Calibration points reset.\n";
}

void CalibrateVideoStreamer::saveCalibrationPoints(const cv::String& filename)
{
    if(!haveSetFourPoints())
        return;

    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
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
    emitter << YAML::EndMap;

    std::ofstream fout(filename);
    fout << emitter.c_str();
    fout.close();

    std::cout << "Calibration points saved to " << filename << ".\n";
    pointsSetSuccessfully = true;
}

void CalibrateVideoStreamer::showCalibrationPoints(cv::Mat& frame)
{
    for(const auto& point : mouseCalibrationPoints)
    {
        cv::circle(
            frame, cv::Point(point.x, point.y), 5, cv::Scalar(0, 255, 255), -1);
    }
}

void CalibrateVideoStreamer::initPreview(TransformPerspective& perspective)
{
    srcPoints.clear();
    srcPoints = mouseCalibrationPoints;
    readCalibSuccess = true;

    initializePerspectiveTransform(perspective);
}

bool CalibrateVideoStreamer::settingCalibrationPoints(cv::Mat& frame)
{
    if(!getNextFrame(frame))
    {
        std::cerr << "Error: Unable to retrieve the next frame.\n";
        return false;
    }

    if(pointsSetSuccessfully)
        return false;

    return true;
}

bool CalibrateVideoStreamer::haveSetFourPoints()
{
    if(mouseCalibrationPoints.size() != 4)
    {
        std::cout
            << "Please set exactly 4 calibration points before proceeding.\n";
        return false;
    }
    else
        return true;
}
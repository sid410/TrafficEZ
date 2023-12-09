#include "CalibrateVideoStreamer.h"
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

CalibrateVideoStreamer::CalibrateVideoStreamer()
    : VideoStreamer()
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

void resetCalibrationPoints(std::vector<cv::Point2f>& points, bool& success)
{
    points.clear();
    success = false;
    std::cout << "Calibration points reset.\n";
}

void saveCalibrationPoints(const std::vector<cv::Point2f>& points,
                           const std::string& filename)
{
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "calibration_points";
    emitter << YAML::Value << YAML::BeginSeq;

    for(const auto& point : points)
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
}

void CalibrateVideoStreamer::setCalibrationPointsFromMouse(
    const std::string& windowName)
{
    std::cout << "Please click on four points (x, y) for calibration.\n"
              << "Press 'r' to reset, or 's' to save and exit.\n";

    std::vector<cv::Point2f> mouseCalibrationPoints;
    bool pointsSetSuccessfully = false;

    cv::setMouseCallback(
        windowName, onMouseClickCallback, &mouseCalibrationPoints);

    cv::Mat frame;
    cv::String calibFilename = "calib_points.yaml";

    while(!pointsSetSuccessfully)
    {
        if(getNextFrame(frame))
        {
            // Draw yellow circles at the existing calibration points
            for(const auto& point : mouseCalibrationPoints)
            {
                cv::circle(frame,
                           cv::Point(point.x, point.y),
                           5,
                           cv::Scalar(0, 255, 255),
                           -1);
            }

            cv::imshow(windowName, frame);

            int key = cv::waitKey(30); // lower this delay for RTSP

            switch(key)
            {
            case 27: // 'Esc' key to exit by interruption
                std::cout << "Calibration interrupted.\n";
                return;
            case 'r': // 'r' key to reset
            case 'R':
                resetCalibrationPoints(mouseCalibrationPoints,
                                       pointsSetSuccessfully);
                break;
            case 's': // 's' key to exit successfully
            case 'S':
                if(mouseCalibrationPoints.size() == 4)
                {
                    pointsSetSuccessfully = true;
                    saveCalibrationPoints(mouseCalibrationPoints,
                                          calibFilename);
                }
                else
                {
                    std::cout << "Please set exactly 4 calibration points "
                                 "before saving and exiting.\n";
                }
                break;
            }
        }
        else
        {
            std::cerr << "Error: Unable to retrieve the next frame.\n";
            break;
        }
    }
}

#include "TrafficManager.h"
#include "CalibrateVideoStreamer.h"
#include "TrimPerspective.h"
#include "VideoStreamer.h"
#include "WarpPerspective.h"
#include <iostream>
#include <opencv2/opencv.hpp>

TrafficManager::TrafficManager(int numCars,
                               int numPedestrians,
                               bool debug,
                               bool calib)
    : numberOfCars(numCars)
    , numberOfPedestrians(numPedestrians)
    , debugMode(debug)
    , calibMode(calib)
{}

void TrafficManager::start()
{
    std::cout << "TrafficManager starting...\n";
    std::cout << "Number of Cars: " << numberOfCars << "\n";
    std::cout << "Number of Pedestrians: " << numberOfPedestrians << "\n";
    std::cout << "Debug Mode: " << (debugMode ? "true" : "false") << "\n";

    if(calibMode)
    {
        calibrateStreamPoints();
    }
    if(debugMode)
    {
        // This is blocking! change this later
        spawnCarObserverDebug();
    }

    std::cout << "TrafficManager ended.\n";
}

void TrafficManager::calibrateStreamPoints()
{
    CalibrateVideoStreamer calibrateStreamer;
    WarpPerspective warpPerspective;
    TrimPerspective trimPerspective;

    cv::String calibWindow = "Calibration Window";
    cv::String previewWindow = "Preview Perspective";
    cv::String calibFilename = "calib_points.yaml";

    if(!calibrateStreamer.openVideoStream("debug.mp4"))
    {
        return;
    }

    cv::Mat frame;
    cv::Mat warpedFrame;
    bool previewToggle = false;
    bool warpToggle = false;

    calibrateStreamer.constructStreamWindow(calibWindow);
    calibrateStreamer.initCalibrationPoints(calibWindow);

    while(calibrateStreamer.settingCalibrationPoints(frame))
    {
        calibrateStreamer.showCalibrationPoints(frame);

        cv::imshow(calibWindow, frame);

        if(previewToggle)
        {
            if(warpToggle) // this is so ugly. change later, for now just debugging
            {
                if(!calibrateStreamer.applyFrameRoi(
                       frame, warpedFrame, warpPerspective))
                    return;
            }
            else
            {
                if(!calibrateStreamer.applyFrameRoi(
                       frame, warpedFrame, trimPerspective))
                    return;
            }

            cv::imshow(previewWindow, warpedFrame);
        }

        int key = cv::waitKey(30);
        switch(key)
        {
        case 27: // 'Esc' key to exit by interruption
            std::cout << "Calibration interrupted.\n";
            return;
        case 'r': // 'r' key to reset
        case 'R':
            calibrateStreamer.resetCalibrationPoints();
            break;
        case 'p': // 'p' key to preview warped frame
        case 'P':
            if(!calibrateStreamer.haveSetFourPoints())
                break;
            previewToggle = !previewToggle;
            if(previewToggle)
            {
                warpToggle = !warpToggle; // debugging just for now
                if(warpToggle)
                    calibrateStreamer.initPreview(frame, warpPerspective);
                else
                    calibrateStreamer.initPreview(frame, trimPerspective);
            }
            else
                cv::destroyWindow(previewWindow);
            break;
        case 's': // 's' key to exit successfully
        case 'S':
            calibrateStreamer.saveCalibrationPoints(calibFilename);
            break;
        }
    }
}

void TrafficManager::spawnCarObserverDebug()
{
    VideoStreamer videoStreamer;
    WarpPerspective warpPerspective;
    TrimPerspective trimPerspective;

    if(!videoStreamer.openVideoStream("debug.mp4"))
    {
        return;
    }

    if(!videoStreamer.readCalibrationPoints("calib_points.yaml"))
    {
        return;
    }

    cv::Mat frame;
    cv::Mat warpedFrame;

    videoStreamer.initializePerspectiveTransform(frame, warpPerspective);

    while(videoStreamer.applyFrameRoi(frame, warpedFrame, warpPerspective))
    {
        cv::imshow("Debug Warped Video", warpedFrame);

        if(cv::waitKey(30) == 27)
            break;
    }

    cv::destroyAllWindows();
}
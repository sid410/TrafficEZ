#include "VideoStreamer.h"
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

VideoStreamer::VideoStreamer()
    : roiMatrixInitialized(false)
    , readCalibSuccess(false)
    , laneLength(0)
    , laneWidth(0)
    , streamWindowInstance("Uninitialized Stream")
{}

VideoStreamer::~VideoStreamer()
{
    stream.release();
    if(cv::getWindowProperty(streamWindowInstance, cv::WND_PROP_VISIBLE) >= 0)
    {
        cv::destroyWindow(streamWindowInstance);
    }
}

/**
 * @brief Opens a stream with cv::VideoCapture method.
 * @param streamName can be video file or link to video stream.
 * @return true if successfully opened.
 */
bool VideoStreamer::openVideoStream(const cv::String& streamName)
{
    stream.open(streamName);

    if(!stream.isOpened())
    {
        std::cerr << "Error: Unable to open stream: " << streamName << "\n";
        return false;
    }

    return true;
}

/**
 * @brief Create and resize an OpenCV window.
 * (There is still no implementation to resize window based on input)
 * @param windowName window label name.
 */
void VideoStreamer::constructStreamWindow(const cv::String& windowName)
{
    originalWidth = static_cast<int>(stream.get(cv::CAP_PROP_FRAME_WIDTH));
    originalHeight = static_cast<int>(stream.get(cv::CAP_PROP_FRAME_HEIGHT));

    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    cv::resizeWindow(windowName, originalWidth, originalHeight);

    streamWindowInstance = windowName;
}

/**
 * @brief Resize an existing window based from a reference frame
 * @param referenceFrame pass here a new frame size, e.g. the warped frame.
 */
void VideoStreamer::resizeStreamWindow(const cv::Mat& referenceFrame)
{
    cv::resizeWindow(
        streamWindowInstance, referenceFrame.cols, referenceFrame.rows);
}

/**
 * @brief Grabs/decodes the next frame from the stream.
 * @param frame the matrix reference to store the next frame.
 * @return true if the next frame is not empty.
 */
bool VideoStreamer::getNextFrame(cv::Mat& frame)
{
    stream.read(frame);
    return !frame.empty();
}

/**
 * @brief Reads the yaml file containing the calibration data.
 * @param yamlFilename the yaml file to open.
 * @return true if successfully parsed the yaml file.
 */
bool VideoStreamer::readCalibrationData(const cv::String& yamlFilename)
{
    std::ifstream fin(yamlFilename);

    try
    {
        if(!fin.is_open())
        {
            std::cerr << "Error: Failed to open calibration file.\n";
            return false;
        }

        YAML::Node yamlNode = YAML::Load(fin);

        const YAML::Node& pointsNode = yamlNode["calibration_points"];
        if(!pointsNode || !pointsNode.IsSequence())
        {
            std::cerr << "Error: Calibration points not found or not in the "
                         "correct format.\n";
            return false;
        }

        // for the four ROI points
        roiPoints.clear();

        for(const auto& point : pointsNode)
        {
            double x = point["x"].as<double>();
            double y = point["y"].as<double>();
            roiPoints.emplace_back(x, y);
        }

        const YAML::Node& dimensionNode = yamlNode["lanes_dimension"];
        if(!dimensionNode || !dimensionNode.IsSequence())
        {
            std::cerr << "Error: Lanes dimensions not found or not in the "
                         "correct format.\n";
            return false;
        }

        // for the lanes total length and width
        this->laneLength = 0;
        this->laneWidth = 0;
        bool dimensionsSet = false;

        for(const auto& dimension : dimensionNode)
        {
            if(dimensionsSet)
            {
                std::cerr
                    << "Error: Multiple lane dimensions found in YAML file.\n";
                return false;
            }

            laneLength = dimension["length"].as<double>();
            laneWidth = dimension["width"].as<double>();

            dimensionsSet = true;
        }

        if(!dimensionsSet)
        {
            std::cerr << "Error: No lane dimensions found in YAML file.\n";
            return false;
        }

        fin.close();
        readCalibSuccess = true;
    }
    catch(const YAML::Exception& e)
    {
        std::cerr << "Error while parsing YAML: " << e.what() << "\n";
        fin.close();
        readCalibSuccess = false;
    }

    return readCalibSuccess;
}

/**
 * @brief Getter for laneLength. Need to first do readCalibrationData
 * @return the total length of the lanes, in meters.
 */
double VideoStreamer::getLaneLength() const
{
    if(laneLength == 0)
    {
        std::cerr << "Error: laneLength is zero.\n";
    }
    return laneLength;
}

/**
 * @brief Getter for laneWidth. Need to first do readCalibrationData
 * @return the total width of the lanes, in meters.
 */
double VideoStreamer::getLaneWidth() const
{
    if(laneWidth == 0)
    {
        std::cerr << "Error: laneWidth is zero.\n";
    }
    return laneWidth;
}

/**
 * @brief Initialize TransformPerspective strategy.
 * @param frame need a reference for frame type and size.
 * @param perspective choose between Warp or Trim.
 */
void VideoStreamer::initializePerspectiveTransform(
    cv::Mat& frame, TransformPerspective& perspective)
{
    if(!readCalibSuccess || roiPoints.size() < 4)
    {
        std::cerr << "Calibration points error.\n";
        return;
    }

    perspective.initialize(frame, roiPoints, roiMatrix);
    roiMatrixInitialized = true;
}

/**
 * @brief Use this to control the while-loop logic to
 * apply the chosen TransformPerspective strategy.
 * This needs an roiMatrix to be initialized first 
 * with initializePerspectiveTransform.
 * @param frame the input frame from stream.
 * @param roiFrame the output frame displaying only ROI.
 * @param perspective choose between Warp or Trim.
 * Important: this should be the same strategy with initialize.
 * @return true if it gets and transformed the next frame to ROI.
 */
bool VideoStreamer::applyFrameRoi(cv::Mat& frame,
                                  cv::Mat& roiFrame,
                                  TransformPerspective& perspective)
{
    if(!roiMatrixInitialized)
    {
        std::cerr << "Error: Failed to initialize.\n";
        return false;
    }

    if(!getNextFrame(frame))
    {
        std::cerr << "Error: Unable to retrieve the next frame.\n";
        return false;
    }

    perspective.apply(frame, roiFrame, roiMatrix);
    return true;
}

cv::Mat VideoStreamer::applyPerspective(cv::Mat inputFrame,
                                        TransformPerspective& perspective)
{
    if(!roiMatrixInitialized)
    {
        std::cerr << "Error: Failed to initialize.\n";
    }

    cv::Mat outputFrame;
    perspective.apply(inputFrame, outputFrame, roiMatrix);

    return outputFrame;
}
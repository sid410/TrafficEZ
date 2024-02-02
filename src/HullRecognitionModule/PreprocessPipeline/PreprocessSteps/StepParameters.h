#ifndef STEP_PARAMETERS_H
#define STEP_PARAMETERS_H

#include <opencv2/core/types.hpp>
#include <variant>

/**
 * @brief Empty struct for Grayscale step as it requires no parameters.
 */
struct GrayscaleParams
{};

/**
 * @brief Parameters for Gaussian Blur preprocessing step.
 * @param kernelSize Kernel size for the blur. Must be positive and odd.
 * @param sigma Gaussian kernel standard deviation in X and Y direction.
 */
struct GaussianBlurParams
{
    int kernelSize = 5;
    double sigma = 9.0;
};

/**
 * @brief Parameters for MOG2 Background Subtraction preprocessing step.
 * @param history Length of the history.
 * @param varThreshold Threshold on the squared Mahalanobis distance.
 * @param varThresholdGen Threshold for the generation of background model.
 * @param nMixtures Number of Gaussian mixtures.
 * @param detectShadows If true, the algorithm will detect shadows.
 * @param shadowValue Value to label shadow pixels in the output.
 */
struct MOG2BackgroundSubtractionParams
{
    int history = 150;
    double varThreshold = 16.0;
    double varThresholdGen = 9.0;
    int nMixtures = 5;
    bool detectShadows = true;
    int shadowValue = 200;
};

/**
 * @brief Parameters for Threshold preprocessing step.
 * @param thresholdValue Threshold value.
 * @param maxValue Maximum value to use with THRESH_BINARY and THRESH_BINARY_INV.
 * @param thresholdType Thresholding type (i.e., enum cv::THRESH_BINARY = 0).
 */
struct ThresholdParams
{
    int thresholdValue = 200;
    int maxValue = 255;
    int thresholdType = 0;
};

/**
 * @brief Parameters for Dilation preprocessing step.
 * @param morphShape Shape of the structuring element (i.e., enum cv::MORPH_ELLIPSE = 2).
 * @param kernelSize Size of the structuring element.
 * @param iterations Number of times dilation is applied.
 */
struct DilationParams
{
    int morphShape = 2;
    cv::Size kernelSize = cv::Size(5, 5);
    int iterations = 4;
};

/**
 * @brief Parameters for Erosion preprocessing step.
 * @param morphShape Shape of the structuring element (i.e., enum cv::MORPH_ELLIPSE = 2).
 * @param kernelSize Size of the structuring element.
 * @param iterations Number of times erosion is applied.
 */
struct ErosionParams
{
    int morphShape = 2;
    cv::Size kernelSize = cv::Size(3, 3);
    int iterations = 5;
};

/**
 * @brief Class to hold various types of preprocessing step parameters.
 *
 * This class uses std::variant to encapsulate parameters for different types
 * of preprocessing steps. It allows for easy and type-safe parameter passing
 * for different preprocessing operations.
 */
class StepParameters
{
public:
    std::variant<GrayscaleParams,
                 GaussianBlurParams,
                 MOG2BackgroundSubtractionParams,
                 ThresholdParams,
                 DilationParams,
                 ErosionParams>
        params;
};

#endif

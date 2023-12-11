#include "TrimPerspective.h"
#include <opencv2/opencv.hpp>

void TrimPerspective::initialize(std::vector<cv::Point2f>& srcPoints,
                                 std::vector<cv::Point2f>& dstPoints,
                                 cv::Mat& perspectiveMatrix)
{}

void TrimPerspective::apply(const cv::Mat& input,
                            cv::Mat& output,
                            cv::Mat& perspectiveMatrix,
                            std::vector<cv::Point2f>& dstPoints)
{
    std::cout << "Trimming...\n";
}
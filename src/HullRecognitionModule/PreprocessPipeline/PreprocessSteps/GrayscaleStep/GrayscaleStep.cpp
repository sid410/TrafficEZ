#include "GrayscaleStep.h"

void GrayscaleStep::process(cv::Mat& frame) const
{
    if(frame.channels() == 1)
        return;
    cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
}

void GrayscaleStep::updateParameters(const StepParameters& params)
{
    std::cout << "No parameters to update for grayscale\n";
}
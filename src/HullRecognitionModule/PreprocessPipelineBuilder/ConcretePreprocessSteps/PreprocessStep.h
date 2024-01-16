#ifndef PREPROCESS_STEP_H
#define PREPROCESS_STEP_H

#include <opencv2/opencv.hpp>

class PreprocessStep
{
public:
    virtual void process(cv::Mat& frame) const = 0;
    virtual ~PreprocessStep() {}
};

#endif
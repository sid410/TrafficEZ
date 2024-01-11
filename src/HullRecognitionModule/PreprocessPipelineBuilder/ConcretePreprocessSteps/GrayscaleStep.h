#ifndef GRAYSCALE_STEP_H
#define GRAYSCALE_STEP_H

#include "PreprocessStep.h"

class GrayscaleStep : public PreprocessStep
{
public:
    void process(cv::Mat& frame) const override;
};

#endif
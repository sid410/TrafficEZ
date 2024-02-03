#ifndef STEP_FACTORY_H
#define STEP_FACTORY_H

#include "IPreprocessStep.h"

class StepFactory
{
public:
    static std::unique_ptr<IPreprocessStep>
    createStep(StepType stepType, const StepParameters& params);
};

#endif

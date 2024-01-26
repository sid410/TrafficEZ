add_library(
  ConcretePreprocessSteps
  DilationStep.cpp ErosionStep.cpp GaussianBlurStep.cpp GrayscaleStep.cpp
  MOG2BackgroundSubtractionStep.cpp ThresholdStep.cpp)

setup_currdir_opencv(ConcretePreprocessSteps)

add_library(
  ConcretePreprocessSteps
  DilationStep.cpp ErosionStep.cpp GaussianBlurStep.cpp GrayscaleStep.cpp
  MOG2BackgroundSubtractionStep.cpp ThresholdStep.cpp)

target_include_directories(
  ConcretePreprocessSteps PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                 ${OpenCV_INCLUDE_DIRS})

target_link_libraries(ConcretePreprocessSteps PRIVATE ${OpenCV_LIBS})

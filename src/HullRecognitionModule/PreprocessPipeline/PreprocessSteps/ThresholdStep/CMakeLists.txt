add_library(ThresholdStep ThresholdStep.cpp)
setup_currdir_opencv(ThresholdStep)

target_include_directories(ThresholdStep PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

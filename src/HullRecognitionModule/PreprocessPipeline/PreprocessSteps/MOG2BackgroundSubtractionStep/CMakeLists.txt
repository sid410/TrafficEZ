add_library(MOG2BackgroundSubtractionStep MOG2BackgroundSubtractionStep.cpp)
setup_currdir_opencv(MOG2BackgroundSubtractionStep)

target_include_directories(MOG2BackgroundSubtractionStep
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library(GaussianBlurStep GaussianBlurStep.cpp)
setup_currdir_opencv(GaussianBlurStep)

target_include_directories(GaussianBlurStep
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

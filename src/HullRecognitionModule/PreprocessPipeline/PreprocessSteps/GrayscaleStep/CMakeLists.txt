add_library(GrayscaleStep GrayscaleStep.cpp)
setup_currdir_opencv(GrayscaleStep)

target_include_directories(GrayscaleStep PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

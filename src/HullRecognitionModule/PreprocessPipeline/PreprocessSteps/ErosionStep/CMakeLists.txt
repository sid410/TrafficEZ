add_library(ErosionStep ErosionStep.cpp)
setup_currdir_opencv(ErosionStep)

target_include_directories(ErosionStep PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

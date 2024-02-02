add_library(DilationStep DilationStep.cpp)
setup_currdir_opencv(DilationStep)

target_include_directories(DilationStep PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

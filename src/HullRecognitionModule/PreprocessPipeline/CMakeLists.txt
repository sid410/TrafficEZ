add_subdirectory(PreprocessSteps)

add_library(PreprocessPipeline PipelineBuilder.cpp PipelineDirector.cpp
                               PipelineTrackbar.cpp)
setup_currdir_opencv(PreprocessPipeline)

target_include_directories(PreprocessPipeline
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/PreprocessSteps)

target_link_libraries(PreprocessPipeline PRIVATE StepFactory)

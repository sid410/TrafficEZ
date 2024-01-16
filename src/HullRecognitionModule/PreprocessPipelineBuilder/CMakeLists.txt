add_subdirectory(ConcretePreprocessSteps)

add_library(PreprocessPipelineBuilder PreprocessPipelineBuilder.cpp)

target_include_directories(
  PreprocessPipelineBuilder
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
         ${CMAKE_CURRENT_SOURCE_DIR}/ConcretePreprocessSteps
         ${OpenCV_INCLUDE_DIRS})

target_link_libraries(PreprocessPipelineBuilder PRIVATE ConcretePreprocessSteps
                                                        ${OpenCV_LIBS})

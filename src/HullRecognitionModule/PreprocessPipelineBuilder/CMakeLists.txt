add_library(PreprocessPipelineBuilder PreprocessPipelineBuilder.cpp)

target_include_directories(
  PreprocessPipelineBuilder
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
         ${CMAKE_CURRENT_SOURCE_DIR}/ConcretePreprocessSteps)

target_link_libraries(PreprocessPipelineBuilder PRIVATE ConcretePreprocessSteps)

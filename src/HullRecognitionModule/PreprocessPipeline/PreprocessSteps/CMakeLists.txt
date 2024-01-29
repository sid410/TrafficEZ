add_library(PreprocessSteps INTERFACE)

set(CONCRETE_STEPS DilationStep ErosionStep GaussianBlurStep GrayscaleStep
                   MOG2BackgroundSubtractionStep ThresholdStep)

foreach(STEP_DIR ${CONCRETE_STEPS})
  add_subdirectory(${STEP_DIR})
  list(APPEND STEP_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/${STEP_DIR})
endforeach()

target_link_libraries(PreprocessSteps INTERFACE ${CONCRETE_STEPS})

target_include_directories(PreprocessSteps INTERFACE ${STEP_INCLUDE_DIRS})

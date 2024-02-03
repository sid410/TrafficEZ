set(CONCRETE_STEPS DilationStep ErosionStep GaussianBlurStep GrayscaleStep
                   MOG2BackgroundSubtractionStep ThresholdStep)

foreach(STEP_DIR ${CONCRETE_STEPS})
  add_subdirectory(${STEP_DIR})
  list(APPEND STEP_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/${STEP_DIR})
endforeach()

add_library(StepFactory StepFactory.cpp)
setup_currdir_opencv(StepFactory)

target_include_directories(StepFactory PUBLIC ${STEP_INCLUDE_DIRS})

target_link_libraries(StepFactory PRIVATE ${CONCRETE_STEPS})

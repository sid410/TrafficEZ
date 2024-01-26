include(CheckCXXCompilerFlag)

# Set C++ standard
set(COMMON_CXX_STANDARD 17)
set(COMMON_CXX_FLAGS "")

check_cxx_compiler_flag("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX17)
  list(APPEND COMMON_CXX_FLAGS "-std=c++17")
endif()

# Compile options for other functions to follow
function(setup_common_compile_options TARGET)
  target_compile_features(${TARGET} PRIVATE cxx_std_${COMMON_CXX_STANDARD})
  target_compile_options(${TARGET} PRIVATE ${COMMON_CXX_FLAGS})
endfunction()

function(setup_currdir_opencv TARGET)
  setup_common_compile_options(${TARGET})
  target_include_directories(${TARGET} PUBLIC ${OpenCV_INCLUDE_DIRS}
                                              ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(${TARGET} PRIVATE ${OpenCV_LIBS})
endfunction()

function(setup_yaml_libstatic TARGET)
  setup_common_compile_options(${TARGET})
  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_SOURCE_DIR}/util/yaml-cpp/include)
  target_link_libraries(
    ${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/util/yaml-cpp/lib/libyaml-cpp.a)
endfunction()

function(setup_fps_helper TARGET)
  setup_common_compile_options(${TARGET})
  target_include_directories(${TARGET}
                             PUBLIC ${CMAKE_SOURCE_DIR}/util/FPSHelper)
  target_link_libraries(${TARGET} PRIVATE FPSHelper)
endfunction()

function(setup_videostreamer TARGET)
  setup_common_compile_options(${TARGET})
  target_include_directories(
    ${TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/src/VideoStreamer
                     ${CMAKE_SOURCE_DIR}/src/VideoStreamer/TransformPerspective)
  target_link_libraries(${TARGET} PRIVATE VideoStreamer TransformPerspective)
endfunction()

function(setup_videostreamer_calib TARGET)
  setup_common_compile_options(${TARGET})
  target_include_directories(
    ${TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/src/VideoStreamer
                     ${CMAKE_SOURCE_DIR}/src/VideoStreamer/TransformPerspective)
  target_link_libraries(${TARGET} PRIVATE CalibrateVideoStreamer
                                          TransformPerspective)
endfunction()

function(setup_hullrecognition TARGET)
  setup_common_compile_options(${TARGET})
  target_include_directories(
    ${TARGET}
    PUBLIC
      ${CMAKE_SOURCE_DIR}/src/HullRecognitionModule/HullDetector
      ${CMAKE_SOURCE_DIR}/src/HullRecognitionModule/HullTracker
      ${CMAKE_SOURCE_DIR}/src/HullRecognitionModule/PreprocessPipelineBuilder)
  target_link_libraries(${TARGET} PRIVATE HullDetector HullTracker
                                          PreprocessPipelineBuilder)
endfunction()

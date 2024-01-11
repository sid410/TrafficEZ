add_library(VehicleWatcher VehicleWatcher.cpp VehicleGui.cpp
                           VehicleHeadless.cpp)

target_include_directories(
  VehicleWatcher
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../VideoStreamer
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../VideoStreamer/TransformPerspective
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../FPSHelper
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../HullRecognitionModule/PreprocessPipelineBuilder
)

target_link_libraries(
  VehicleWatcher PRIVATE VideoStreamer TransformPerspective FPSHelper
                         PreprocessPipelineBuilder)

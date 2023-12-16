add_library(CalibrateWatcher CalibrateWatcher.cpp CalibrateGui.cpp
                             CalibrateHeadless.cpp)

target_include_directories(
  CalibrateWatcher
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
         ${CMAKE_CURRENT_SOURCE_DIR}/../../../VideoStreamer
         ${CMAKE_CURRENT_SOURCE_DIR}/../../../VideoStreamer/TransformPerspective
)

target_link_libraries(CalibrateWatcher PRIVATE CalibrateVideoStreamer
                                               TransformPerspective)

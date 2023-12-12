add_library(TrafficManager TrafficManager.cpp)

target_include_directories(
  TrafficManager
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
         ${CMAKE_CURRENT_SOURCE_DIR}/../VideoStreamer
         ${CMAKE_CURRENT_SOURCE_DIR}/../VideoStreamer/TransformPerspective)

target_link_libraries(
  TrafficManager PRIVATE VideoStreamer CalibrateVideoStreamer
                         TransformPerspective)

add_library(TrafficManager TrafficManager.cpp)

target_include_directories(
  TrafficManager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/../TrafficVideoStreamer)

target_link_libraries(TrafficManager PRIVATE TrafficVideoStreamer)
target_link_libraries(TrafficManager PRIVATE yaml-cpp)

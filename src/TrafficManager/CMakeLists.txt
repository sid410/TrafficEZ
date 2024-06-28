add_library(TrafficManager TrafficManager.cpp)

target_include_directories(
  TrafficManager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/../MultiprocessTraffic)

target_link_libraries(TrafficManager PRIVATE WatcherSpawner MultiprocessTraffic)

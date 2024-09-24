add_library(TrafficManager TrafficManager.cpp)

target_include_directories(
  TrafficManager
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
         ${CMAKE_CURRENT_SOURCE_DIR}/../WatcherSpawner
         ${CMAKE_CURRENT_SOURCE_DIR}/../MultiprocessTraffic
         ${CMAKE_CURRENT_SOURCE_DIR}/../IotHubClient)

target_link_libraries(TrafficManager PRIVATE WatcherSpawner MultiprocessTraffic IotHubClient)

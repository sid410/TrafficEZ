add_library(MultiprocessTraffic MultiprocessTraffic.cpp ParentProcess.cpp
                                ChildProcess.cpp PhaseMessageType.cpp)
setup_yaml_libstatic(MultiprocessTraffic)
setup_azureiothub(MultiprocessTraffic)

target_include_directories(
  MultiprocessTraffic
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
         ${CMAKE_CURRENT_SOURCE_DIR}/../WatcherSpawner
         ${CMAKE_CURRENT_SOURCE_DIR}/../RelayController)

target_link_libraries(MultiprocessTraffic PRIVATE WatcherSpawner
                                                  RelayController)

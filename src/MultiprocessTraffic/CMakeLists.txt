add_library(MultiprocessTraffic MultiprocessTraffic.cpp ParentProcess.cpp
                                ChildProcess.cpp PhaseMessageType.cpp)
setup_yaml_libstatic(MultiprocessTraffic)
setup_boost_log(MultiprocessTraffic)

target_include_directories(
  MultiprocessTraffic
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
         ${CMAKE_CURRENT_SOURCE_DIR}/../WatcherSpawner
         ${CMAKE_CURRENT_SOURCE_DIR}/../RelayController
         ${CMAKE_CURRENT_SOURCE_DIR}/../Reports)

target_link_libraries(MultiprocessTraffic PRIVATE WatcherSpawner
                                                  RelayController Reports)

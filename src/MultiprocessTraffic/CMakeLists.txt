add_library(MultiprocessTraffic MultiprocessTraffic.cpp ParentProcess.cpp ChildProcess.cpp PhaseMessageType.cpp)
setup_yaml_libstatic(MultiprocessTraffic)

target_include_directories(
    MultiprocessTraffic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/../WatcherSpawner)

target_link_libraries(MultiprocessTraffic PRIVATE WatcherSpawner)

add_library(MultiprocessTraffic MultiprocessTraffic.cpp ParentProcess.cpp ChildProcess.cpp)

target_include_directories(
    MultiprocessTraffic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/../WatcherSpawner)

target_link_libraries(MultiprocessTraffic PRIVATE WatcherSpawner)

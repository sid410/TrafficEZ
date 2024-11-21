# Add here all the directory of submodules
add_subdirectory(TrafficManager)
add_subdirectory(VideoStreamer)
add_subdirectory(WatcherSpawner)
add_subdirectory(HullRecognitionModule)
add_subdirectory(SegmentationModule)
add_subdirectory(MultiprocessTraffic)
add_subdirectory(RelayController)
add_subdirectory(Reports)

add_executable(${EXECUTABLE_NAME} main.cpp)
setup_currdir_opencv(${EXECUTABLE_NAME})

target_include_directories(${EXECUTABLE_NAME}
                           PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE TrafficManager)

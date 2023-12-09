# Add here all the directory of submodules
add_subdirectory(TrafficManager)
add_subdirectory(VideoStreamer)

add_executable(${EXECUTABLE_NAME} main.cpp)

# Link the 3rd party libraries
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${OpenCV_LIBS})
# only link yaml-cpp to where we need to parse yaml files
# target_link_libraries(${EXECUTABLE_NAME} PRIVATE yaml-cpp)

# Add here, link all submodule libraries
target_link_libraries(${EXECUTABLE_NAME} PRIVATE TrafficManager)

# Include OpenCV and project Root
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_include_directories(${EXECUTABLE_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

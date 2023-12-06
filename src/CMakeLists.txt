# Add here all the directory of submodules
add_subdirectory(TrafficManager)
add_subdirectory(TrafficVideoStreamer)

add_executable(${EXECUTABLE_NAME} main.cpp)

# Link the OpenCV libraries
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${OpenCV_LIBS})

# Add here, link all submodule libraries
target_link_libraries(${EXECUTABLE_NAME} PRIVATE TrafficManager)

# Include OpenCV and project Root
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_include_directories(${EXECUTABLE_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

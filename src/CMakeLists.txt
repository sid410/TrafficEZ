add_executable(${EXECUTABLE_NAME} main.cpp)

# Link the OpenCV libraries
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${OpenCV_LIBS})

# Include OpenCV and project Root
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
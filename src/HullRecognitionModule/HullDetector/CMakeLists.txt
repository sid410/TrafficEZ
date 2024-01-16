add_library(HullDetector HullDetector.cpp)

target_include_directories(HullDetector PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                               ${OpenCV_INCLUDE_DIRS})

target_link_libraries(HullDetector PRIVATE ${OpenCV_LIBS})

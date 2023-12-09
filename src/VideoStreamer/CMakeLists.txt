add_library(VideoStreamer VideoStreamer.cpp)
target_include_directories(VideoStreamer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                                ${OpenCV_INCLUDE_DIRS})

target_link_libraries(VideoStreamer PRIVATE ${OpenCV_LIBS})
target_link_libraries(VideoStreamer PRIVATE yaml-cpp)

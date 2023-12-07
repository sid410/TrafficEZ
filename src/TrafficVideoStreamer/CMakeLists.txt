add_library(TrafficVideoStreamer TrafficVideoStreamer.cpp)
target_include_directories(
  TrafficVideoStreamer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                              ${OpenCV_INCLUDE_DIRS})

target_link_libraries(TrafficVideoStreamer PRIVATE ${OpenCV_LIBS})

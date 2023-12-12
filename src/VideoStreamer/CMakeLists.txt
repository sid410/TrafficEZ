add_library(VideoStreamer VideoStreamer.cpp)
add_library(CalibrateVideoStreamer CalibrateVideoStreamer.cpp)
add_library(TransformPerspective TransformPerspective.cpp WarpPerspective.cpp
                                 TrimPerspective.cpp)

target_include_directories(VideoStreamer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                                ${OpenCV_INCLUDE_DIRS})

target_include_directories(
  CalibrateVideoStreamer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                ${OpenCV_INCLUDE_DIRS})

target_include_directories(
  TransformPerspective PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                              ${OpenCV_INCLUDE_DIRS})

target_link_libraries(VideoStreamer PRIVATE ${OpenCV_LIBS} yaml-cpp
                                            TransformPerspective)
target_link_libraries(CalibrateVideoStreamer PRIVATE ${OpenCV_LIBS} yaml-cpp
                                                     TransformPerspective)

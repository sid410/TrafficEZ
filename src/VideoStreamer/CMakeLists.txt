add_subdirectory(TransformPerspective)

add_library(VideoStreamer VideoStreamer.cpp)
add_library(CalibrateVideoStreamer CalibrateVideoStreamer.cpp)

target_include_directories(VideoStreamer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                                ${OpenCV_INCLUDE_DIRS}
                                                ${CMAKE_CURRENT_SOURCE_DIR}/../../util/yaml-cpp/include)

target_include_directories(
  CalibrateVideoStreamer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                ${OpenCV_INCLUDE_DIRS}
                                ${CMAKE_CURRENT_SOURCE_DIR}/../../util/yaml-cpp/include)

target_link_libraries(VideoStreamer PRIVATE ${OpenCV_LIBS} TransformPerspective
                                            ${CMAKE_CURRENT_SOURCE_DIR}/../../util/yaml-cpp/lib/libyaml-cpp.a)
target_link_libraries(CalibrateVideoStreamer PRIVATE ${OpenCV_LIBS} TransformPerspective
                                            ${CMAKE_CURRENT_SOURCE_DIR}/../../util/yaml-cpp/lib/libyaml-cpp.a)
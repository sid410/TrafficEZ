add_library(TransformPerspective TransformPerspective.cpp WarpPerspective.cpp
                                 TrimPerspective.cpp)

target_include_directories(
  TransformPerspective PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                              ${OpenCV_INCLUDE_DIRS})

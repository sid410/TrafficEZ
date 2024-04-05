add_library(onnxruntime STATIC IMPORTED)

set_target_properties(
  onnxruntime PROPERTIES IMPORTED_LOCATION
                         ${CMAKE_CURRENT_SOURCE_DIR}/lib/libonnxruntime.so)

target_include_directories(onnxruntime
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(yaml-cpp STATIC IMPORTED)

set_target_properties(
  yaml-cpp PROPERTIES IMPORTED_LOCATION
                      ${CMAKE_CURRENT_SOURCE_DIR}/lib/libyaml-cpp.a)

target_include_directories(yaml-cpp
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

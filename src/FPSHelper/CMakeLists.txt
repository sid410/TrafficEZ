add_library(FPSHelper FPSHelper.cpp)

target_include_directories(FPSHelper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                            ${OpenCV_INCLUDE_DIRS})

target_link_libraries(FPSHelper PRIVATE ${OpenCV_LIBS})

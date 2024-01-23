add_library(HullTracker HullTracker.cpp HullTrackable.cpp)

target_include_directories(HullTracker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                               ${OpenCV_INCLUDE_DIRS} 
                                               ${CMAKE_CURRENT_SOURCE_DIR}/../../FPSHelper)

target_link_libraries(HullTracker PRIVATE ${OpenCV_LIBS} FPSHelper)

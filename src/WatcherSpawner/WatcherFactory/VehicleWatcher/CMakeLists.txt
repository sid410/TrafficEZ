add_library(VehicleWatcher VehicleWatcher.cpp VehicleGui.cpp
                           VehicleHeadless.cpp)

setup_videostreamer(VehicleWatcher)
setup_hullrecognition(VehicleWatcher)
setup_fps_helper(VehicleWatcher)
setup_ort_api(VehicleWatcher)
setup_ort_segmentation(VehicleWatcher)

target_include_directories(VehicleWatcher
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

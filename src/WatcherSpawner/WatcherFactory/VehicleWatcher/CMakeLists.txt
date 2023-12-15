add_library(VehicleWatcher VehicleWatcher.cpp VehicleGui.cpp
                           VehicleHeadless.cpp)

target_include_directories(VehicleWatcher
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_library(CalibrateWatcher CalibrateWatcher.cpp CalibrateGui.cpp
                             CalibrateHeadless.cpp)

target_include_directories(CalibrateWatcher
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

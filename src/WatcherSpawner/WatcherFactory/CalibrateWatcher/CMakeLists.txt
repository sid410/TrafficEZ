add_library(CalibrateWatcher CalibrateWatcher.cpp CalibrateGui.cpp
                             CalibrateHeadless.cpp)
setup_videostreamer_calib(CalibrateWatcher)

target_include_directories(CalibrateWatcher
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

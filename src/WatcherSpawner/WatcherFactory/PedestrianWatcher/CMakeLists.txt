add_library(PedestrianWatcher PedestrianWatcher.cpp PedestrianGui.cpp
                              PedestrianHeadless.cpp)

setup_videostreamer(PedestrianWatcher)
setup_ort_api(PedestrianWatcher)
setup_ort_segmentation(PedestrianWatcher)

target_include_directories(PedestrianWatcher
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

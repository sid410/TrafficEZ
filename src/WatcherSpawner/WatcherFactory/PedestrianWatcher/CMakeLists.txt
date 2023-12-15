add_library(PedestrianWatcher PedestrianWatcher.cpp PedestrianGui.cpp
                              PedestrianHeadless.cpp)

target_include_directories(PedestrianWatcher
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

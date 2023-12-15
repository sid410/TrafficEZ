add_subdirectory(VehicleWatcher)
add_subdirectory(PedestrianWatcher)
add_subdirectory(CalibrateWatcher)

add_library(WatcherFactory WatcherFactory.cpp)

target_include_directories(
  WatcherFactory
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/VehicleWatcher
         ${CMAKE_CURRENT_SOURCE_DIR}/PedestrianWatcher
         ${CMAKE_CURRENT_SOURCE_DIR}/CalibrateWatcher)

target_link_libraries(WatcherFactory PRIVATE VehicleWatcher PedestrianWatcher
                                             CalibrateWatcher)

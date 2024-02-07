add_subdirectory(CalibrateWatcher)
add_subdirectory(PedestrianWatcher)
add_subdirectory(VehicleWatcher)

add_library(WatcherFactory WatcherFactory.cpp)

target_include_directories(
  WatcherFactory
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/CalibrateWatcher
         ${CMAKE_CURRENT_SOURCE_DIR}/PedestrianWatcher
         ${CMAKE_CURRENT_SOURCE_DIR}/VehicleWatcher)

target_link_libraries(WatcherFactory PRIVATE CalibrateWatcher PedestrianWatcher
                                             VehicleWatcher)

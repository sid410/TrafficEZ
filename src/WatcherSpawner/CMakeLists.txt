add_subdirectory(WatcherFactory)

add_library(WatcherSpawner WatcherSpawner.cpp)

target_include_directories(
  WatcherSpawner
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/WatcherFactory
         ${CMAKE_CURRENT_SOURCE_DIR}/WatcherFactory/include
         ${CMAKE_CURRENT_SOURCE_DIR}/WatcherFactory/CalibrateWatcher
         ${CMAKE_CURRENT_SOURCE_DIR}/WatcherFactory/PedestrianWatcher
         ${CMAKE_CURRENT_SOURCE_DIR}/WatcherFactory/VehicleWatcher)

target_link_libraries(WatcherSpawner PRIVATE WatcherFactory)

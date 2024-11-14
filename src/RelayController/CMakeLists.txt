add_library(RelayController HttpRelayController.cpp TelnetRelayController.cpp)

target_include_directories(
  RelayController PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                         ${CMAKE_CURRENT_SOURCE_DIR}/../MultiprocessTraffic)

target_link_libraries(RelayController PRIVATE curl)

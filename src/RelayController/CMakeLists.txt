add_library(RelayController RelayController.cpp)

target_include_directories(
    RelayController PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/../MultiprocessTraffic)
                        # for the PhaseMessageType

target_link_libraries(RelayController CURL::libcurl)
add_library(TrafficManager TrafficManager.cpp)

target_include_directories(
  TrafficManager
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
         ${CMAKE_CURRENT_SOURCE_DIR}/../WatcherSpawner
         ${CMAKE_CURRENT_SOURCE_DIR}/../MultiprocessTraffic)

find_library(AZURE_IOTHUB_CLIENT iothub_client REQUIRED)
find_library(AZURE_IOTHUB_MESSAGE iothub_message REQUIRED)
find_library(AZURE_IOTHUB_TRANSPORT_MQTT iothubtransportmqtt REQUIRED)
find_library(AZURE_C_SHARED_UTILITY platform REQUIRED)

# Link libraries
target_link_libraries(
  TrafficManager 
  PRIVATE 
    WatcherSpawner 
    MultiprocessTraffic
    ${AZURE_IOTHUB_CLIENT} 
    ${AZURE_IOTHUB_MESSAGE} 
    ${AZURE_IOTHUB_TRANSPORT_MQTT} 
    ${AZURE_C_SHARED_UTILITY}
)

target_link_libraries(TrafficManager PRIVATE WatcherSpawner MultiprocessTraffic)

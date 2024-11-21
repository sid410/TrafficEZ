add_library(Reports HttpClient.cpp Reports.cpp)

target_link_libraries(Reports PRIVATE cpprest)

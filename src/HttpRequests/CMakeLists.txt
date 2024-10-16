add_library(
    HttpRequests
        HttpPostClient.cpp
        HttpPostClientAsync.cpp)

target_link_libraries(HttpRequests CURL::libcurl)

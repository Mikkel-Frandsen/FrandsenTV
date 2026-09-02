find_package(CURL REQUIRED)

add_library(http
        HttpClient.cpp
        HttpClient.h
        HttpResponse.h
)

target_link_libraries(http
        PRIVATE
        CURL::libcurl
        Qt6::Core
)

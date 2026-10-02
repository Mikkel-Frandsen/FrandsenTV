find_package(CURL REQUIRED)

add_library(http
        HttpClient.cpp
        HttpClient.h
        HttpResponse.h
        TMDBManager.cpp
        TMDBManager.h
)


target_include_directories(http
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(http
        PRIVATE
        CURL::libcurl
        Qt6::Core
        domain
)

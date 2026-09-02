find_package(Qt6 REQUIRED COMPONENTS Core Network Sql Widgets)
find_package(CURL REQUIRED)

add_executable(tests
        main.cpp

)

target_link_libraries(tests
        PRIVATE
        CURL::libcurl
        Qt6::Core
        Qt6::Network
        Qt6::Sql
        Qt6::Widgets
        playback
)
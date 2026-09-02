find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_library(localServer
        localServer.cpp
        localServer.h
)

target_include_directories(localServer
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(localServer
        PUBLIC
        Qt6::Core
        Qt6::Network
)
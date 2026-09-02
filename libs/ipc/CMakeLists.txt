add_library(ipc
        localServer.cpp
        localServer.h
        localClient.cpp
        localClient.h

)

target_include_directories(ipc
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ipc
        PUBLIC
        Qt6::Core
        Qt6::Network
        protocol
)
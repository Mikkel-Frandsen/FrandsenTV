add_library(protocol
        protocol.h
        protocol.cpp
)

target_include_directories(protocol
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(protocol
        PUBLIC
        Qt6::Core


)
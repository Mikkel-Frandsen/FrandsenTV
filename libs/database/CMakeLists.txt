add_library(database STATIC
        DatabaseManager.cpp
)

target_include_directories(database
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(database
        PUBLIC
        Qt6::Sql
        domain
        protocol
)
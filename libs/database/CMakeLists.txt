add_library(database STATIC
        DatabaseManager.cpp
        CatalogRep.cpp
        CatalogRep.h
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
        http
)
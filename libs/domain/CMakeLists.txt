add_library(domain
        movie.cpp
        movie.h
        catalog.cpp
        catalog.h
)

target_include_directories(domain
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(domain
        PUBLIC
        Qt6::Core
)
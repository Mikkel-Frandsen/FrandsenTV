find_package(CURL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Sql)
add_executable(smarttv-sync
        main.cpp
        SyncManager.cpp
        SyncManager.h
        ../../libs/database/DatabaseManager.h
        ../../libs/database/DatabaseManager.cpp


)

target_link_libraries(smarttv-sync
        PRIVATE
        CURL::libcurl
        Qt6::Sql
        Qt6::Core
        protocol
        database
        domain
)


find_package(Qt6 REQUIRED COMPONENTS Core Network)
find_package(CURL)

qt_standard_project_setup()

qt_add_executable(smarttv-core
        main.cpp
        CoreServer.cpp
        CoreServer.h
        ../../libs/database/DatabaseManager.cpp
        ../../libs/database/DatabaseManager.h
        ../sync/SyncManager.cpp
        ../sync/SyncManager.h

)

target_link_libraries(smarttv-core
        PRIVATE
        Qt6::Core
        Qt6::Network
        Qt6::Sql
        CURL::libcurl
        protocol
        ipc
        domain
        database
        playback
)
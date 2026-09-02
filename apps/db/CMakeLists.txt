find_package(Qt6 REQUIRED COMPONENTS
        Quick
        Network
)

qt_standard_project_setup()

qt_add_executable(smarttv-db
        main.cpp
        DatabaseServer.cpp
        DatabaseServer.h
        ../../libs/protocol/databaseProtocol.cpp
        ../../libs/protocol/databaseProtocol.h
        ../../libs/ipc/localServer.cpp
        ../../libs/ipc/localServer.h
)

target_link_libraries(smarttv-db
        PRIVATE
        Qt6::Quick
        Qt6::Network
        protocol
)

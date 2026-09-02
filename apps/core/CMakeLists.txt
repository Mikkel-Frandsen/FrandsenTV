find_package(Qt6 REQUIRED COMPONENTS Core Network)

qt_standard_project_setup()

qt_add_executable(smarttv-core
        main.cpp
        CoreServer.cpp
        CoreServer.h
        ../../libs/protocol/coreProtocol.cpp
        ../../libs/protocol/coreProtocol.h
        ../../libs/protocol/databaseProtocol.cpp
        ../../libs/protocol/databaseProtocol.h
        ../../libs/ipc/localServer.cpp
        ../../libs/ipc/localServer.h
        ../../libs/ipc/localClient.cpp
        ../../libs/ipc/localClient.h


)

target_link_libraries(smarttv-core
        PRIVATE
        Qt6::Core
        Qt6::Network
        protocol
        localServer
)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

qt_standard_project_setup()

qt_add_executable(smarttv-core
        main.cpp
        CoreServer.cpp
        CoreServer.h

)

target_link_libraries(smarttv-core
        PRIVATE
        Qt6::Core
        Qt6::Network
)

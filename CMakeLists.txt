cmake_minimum_required(VERSION 3.16)

project(SmartTV
        VERSION 1.0
        LANGUAGES CXX
)

find_package(Qt6 REQUIRED COMPONENTS
        Core
        Network
        Sql
)

qt_standard_project_setup()

add_subdirectory(libs/protocol)
add_subdirectory(libs/ipc)
add_subdirectory(libs/domain)
add_subdirectory(libs/database)
add_subdirectory(libs/playback)
add_subdirectory(libs/http)





add_subdirectory(apps/sync)
add_subdirectory(apps/core)
add_subdirectory(apps/ui)

add_subdirectory(tests)


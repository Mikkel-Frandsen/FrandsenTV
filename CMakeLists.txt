cmake_minimum_required(VERSION 3.26)

project(SmartTV
        VERSION 0.1.0
        LANGUAGES CXX
)



set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(core)
add_subdirectory(sync)

add_subdirectory(apps/core)
add_subdirectory(apps/sync)
add_subdirectory(apps/ui)

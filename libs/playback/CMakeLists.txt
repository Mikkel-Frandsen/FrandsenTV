find_package(PkgConfig REQUIRED)
pkg_check_modules(MPV REQUIRED IMPORTED_TARGET mpv)

add_library(playback
        RealDebridClient.cpp
        RealDebridClient.h
        stream.h
        streamProvider.h
        TorrentioProvider.cpp
        TorrentioProvider.h
        PlaybackManager.cpp
        PlaybackManager.h


        Player/Player.h
        Player/MpvPlayer.h
        Player/MpvPlayer.cpp
)

target_include_directories(playback
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(playback
        PUBLIC
        Qt6::Core
        Qt6::Network
        Qt6::Sql
        protocol
        PkgConfig::MPV
        http
)
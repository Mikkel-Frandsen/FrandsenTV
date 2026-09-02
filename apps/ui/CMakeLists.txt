find_package(Qt6 REQUIRED COMPONENTS
        Quick
        Network
        Core
)

qt_standard_project_setup()

qt_add_executable(smarttv-ui
        main.cpp
        CoreClient.cpp
        CoreClient.h
)

qt_add_qml_module(smarttv-ui
        URI SmartTV
        VERSION 1.0

        QML_FILES
        qml/Main.qml
        qml/components/NavigationBar.qml
        qml/components/MediaCard.qml
        qml/components/MediaSection.qml
        qml/pages/HomePage.qml
        qml/pages/MoviesPage.qml

)

target_link_libraries(smarttv-ui
        PRIVATE
        Qt6::Quick
        Qt6::Network
        Qt6::Core
        protocol
        ipc
)
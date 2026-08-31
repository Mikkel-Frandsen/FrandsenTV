find_package(Qt6 REQUIRED COMPONENTS
        Quick
        Network
)

qt_standard_project_setup()

qt_add_executable(smarttv-ui
        main.cpp
        AppModel.cpp
        AppModel.h
)

qt_add_qml_module(smarttv-ui
        URI SmartTV
        VERSION 1.0

        QML_FILES
        qml/Main.qml
)

target_link_libraries(smarttv-ui
        PRIVATE
        Qt6::Quick
        Qt6::Network
)

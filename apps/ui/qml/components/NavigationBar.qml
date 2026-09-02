import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property int currentPage: 0
    property bool connectedToCore: false

    signal pageSelected(int page)

    height: 80

    color: "white"
    border.color: "black"
    border.width: 2

    RowLayout {
        anchors.fill: parent

        anchors.leftMargin: 25
        anchors.rightMargin: 25

        spacing: 25

        Text {
            text: "FRANDSEN TV"

            color: "black"
            font.pixelSize: 24
            font.bold: true
        }

        Repeater {
            model: [
                "Home",
                "TV Shows",
                "Movies",
                "Games",
                "Apps"
            ]

            delegate: Button {
                text: modelData

                flat: true

                font.pixelSize: 18

                contentItem: Text {
                    text: parent.text

                    color: "black"

                    font.pixelSize: 18
                    font.bold: root.currentPage === index

                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle {
                    color: "white"

                    border.color: root.currentPage === index
                        ? "black"
                        : "transparent"

                    border.width: 2
                }

                onClicked: {
                    root.pageSelected(index)
                }
            }
        }

        Item {
            Layout.fillWidth: true
        }

        Text {
            text: root.connectedToCore
                ? "CORE: ONLINE"
                : "CORE: OFFLINE"

            color: "black"

            font.pixelSize: 15
            font.bold: true
        }
    }
}
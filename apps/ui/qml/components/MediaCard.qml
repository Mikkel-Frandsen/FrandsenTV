import QtQuick
import QtQuick.Controls

FocusScope {
    id: root

    property string title: ""
    property string subtitle: ""
    property string posterPath: ""
    property string content_id: ""

    signal clicked()
    signal playClicked(string id)

    width: 190
    height: 285

    Rectangle {
        anchors.fill: parent

        color: "white"
        border.color: "black"
        border.width: root.activeFocus ? 4 : 2

        Column {
            anchors.fill: parent
            spacing: 0

            Rectangle {
                width: parent.width
                height: 205

                color: "white"
                border.color: "black"
                border.width: 1

                Image {
                    id: poster

                    anchors.fill: parent

                    source: root.posterPath !== ""
                        ? "https://image.tmdb.org/t/p/w342" + root.posterPath
                        : ""

                    fillMode: Image.PreserveAspectCrop
                    visible: status === Image.Ready
                    asynchronous: true
                    cache: true

                    onStatusChanged: {
                        //console.log("Poster:", source, "status:", status)
                    }
                }

                Text {
                    anchors.centerIn: parent

                    text: poster.status === Image.Error
                        ? "IMAGE ERROR"
                        : "MOVIE"

                    color: "black"
                    font.pixelSize: 16
                    font.bold: true

                    visible: poster.status !== Image.Ready
                }
            }

            Column {
                width: parent.width
                spacing: 4

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                anchors.topMargin: 10

                Text {
                    width: parent.width

                    text: root.title
                    color: "black"

                    font.pixelSize: 17
                    font.bold: true

                    elide: Text.ElideRight
                }

                Text {
                    width: parent.width

                    text: root.subtitle
                    color: "black"

                    font.pixelSize: 14

                    elide: Text.ElideRight
                }
            }
        }

        MouseArea {
            anchors.fill: parent

            onClicked: {
                root.forceActiveFocus()
                root.clicked()
            }
        }

        Button {
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            text: "Play"

            onClicked: {
                root.playClicked(root.content_id)
            }
        }
    }

    Keys.onReturnPressed: root.clicked()
}
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components" as Components

Page {
    background: Rectangle {
        color: "white"
    }

    ScrollView {
        anchors.fill: parent

        Column {
            anchors.fill: parent

            anchors.leftMargin: 40
            anchors.rightMargin: 40
            anchors.topMargin: 30
            anchors.bottomMargin: 30

            spacing: 20

            Text {
                text: "Home"
                color: "black"
                font.pixelSize: 36
                font.bold: true
            }

            Components.MediaSection {
                width: parent.width
                title: "Continue Watching"
                model: []
            }

            Components.MediaSection {
                width: parent.width
                title: "Popular Movies"
                model: []
            }

            Components.MediaSection {
                width: parent.width
                title: "TV Shows"
                model: []
            }

            Components.MediaSection {
                width: parent.width
                title: "Apps"
                model: []
            }
        }
    }
}
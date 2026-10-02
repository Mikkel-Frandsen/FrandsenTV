import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components" as Components

Item {
    id: root

    property var movies: []
    property var catalogs: []

    signal playMovie(string movieId)
    Rectangle {
        anchors.fill: parent
        color: "white"
    }

    ColumnLayout {
        anchors.fill: parent

        anchors.leftMargin: 40
        anchors.rightMargin: 40
        anchors.topMargin: 30
        anchors.bottomMargin: 30

        spacing: 20

        Text {
            text: "Movies"

            color: "black"
            font.pixelSize: 36
            font.bold: true

            Layout.fillWidth: true
        }



        ListView {
            anchors.fill: parent
            anchors.margins: 50
            clip: true

            model: root.catalogs

            header: Item {
                width: ListView.view.width
                height: 300 + 25

                Components.MediaSection {
                    anchors.fill: parent

                    title: "All Movies"
                    model: root.movies

                    onPlayMovie: function(movieId) {
                        root.playMovie(movieId)
                    }
                }
            }

            delegate: Item {
                width: ListView.view.width
                height: mediaSection.implicitHeight + 20

                Components.MediaSection {
                    id: mediaSection
                    width: parent.width
                    anchors.top: parent.top

                    title: modelData.title
                    model: modelData.catalog

                    onPlayMovie: function(movieId) {
                        root.playMovie(movieId)
                    }
                }
            }
        }
    }
}
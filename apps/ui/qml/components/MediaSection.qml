import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: root

    property string title: ""
    property var model

    property int cardWidth: 190
    property int cardHeight: 285

    spacing: 12
    signal playMovie(string movie);
    Text {
        text: root.title

        color: "black"
        font.pixelSize: 24
        font.bold: true

        Layout.fillWidth: true
    }

    ListView {
        id: listView

        Layout.fillWidth: true
        Layout.preferredHeight: root.cardHeight + 10

        orientation: ListView.Horizontal
        spacing: 20

        clip: true

        model: root.model

        delegate: MediaCard {
            width: root.cardWidth
            height: root.cardHeight

            title: modelData.title
            subtitle: modelData.year + " • " + modelData.genre
            posterPath: modelData.poster
            content_id: modelData.content_id

            onClicked: {
                root.forceActiveFocus()
                console.log("Selected:", modelData.title)
            }

            onPlayClicked: {
                console.log("Playing:", modelData.title)

                root.playMovie(modelData.content_id)
            }
        }
    }
}
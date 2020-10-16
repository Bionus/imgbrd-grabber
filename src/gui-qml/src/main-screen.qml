import QtQuick 2.12
import QtQuick.Controls 2.5

ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 300
    height: 500
    title: "Grabber"

    property int page: 1;

    ScrollView {
        anchors.bottomMargin: 40
        anchors.topMargin: 40
        anchors.fill: parent

        Label {
            id: label
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 100
            text: backend.log
        }

        GridView {
            id: results
            y: 100
            width: parent.width
            cellWidth: 100
            cellHeight: 100
            model: backend.thumbnails

            delegate: Image {
                source: modelData.previewUrl
                width: 100
                height: 100
                fillMode: Image.PreserveAspectFit
            }
        }
    }

    Button {
        id: buttonPreviousPage
        width: 40
        text: "<"
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        enabled: backend.query !== "" && page > 1
        onClicked: {
            page--
            backend.search(textFieldSearch.text, page)
        }
    }

    Button {
        id: buttonNextPage
        width: 40
        text: ">"
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        enabled: backend.query !== ""
        onClicked: {
            page++
            backend.search(textFieldSearch.text, page)
        }
    }

    TextField {
        id: textFieldSearch
        text: "rating:safe wallpaper"
        placeholderText: "Search..."
        anchors.right: parent.right
        anchors.rightMargin: 40
        anchors.left: parent.left
        anchors.top: parent.top
    }

    Button {
        id: buttonSources
        text: "Sources"
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.rightMargin: 40
        anchors.left: parent.left
        anchors.leftMargin: 40
    }

    Button {
        id: buttonSearch
        width: 40
        text: "Go"
        anchors.right: parent.right
        anchors.top: parent.top
        onClicked: backend.search(textFieldSearch.text, page)
    }

}

/*##^##
Designer {
    D{i:4}D{i:5}D{i:6}D{i:7}D{i:8}D{i:9}
}
##^##*/

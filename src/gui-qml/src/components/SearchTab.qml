import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.5

import "../vendor"

Item {
    id: root

    signal openSources()
    signal search(string site, string query, int page)

    property int page: 1
    property string site
    property string query
    property var thumbnails

    ScrollView {
        anchors.bottomMargin: 40
        anchors.topMargin: 40
        anchors.fill: parent

        ColumnFlow {
            id: results
            width: parent.width
            columns: 3
            model: thumbnails

            delegate: Image {
                source: modelData.previewUrl
                fillMode: Image.PreserveAspectFit

                onHeightChanged: resultsRefresher.restart()
            }
        }

        Timer {
            id: resultsRefresher
            interval: 100
            running: false
            repeat: false

            onTriggered: results.reEvalColumns()
        }
    }

    Button {
        width: 40
        text: "<"
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        enabled: query !== "" && page > 1

        onClicked: {
            page--
            root.search(site, textFieldSearch.text, page)
        }
    }

    Button {
        width: 40
        text: ">"
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        enabled: query !== ""

        onClicked: {
            page++
            root.search(site, textFieldSearch.text, page)
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
        text: "Sources"
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.rightMargin: 40
        anchors.left: parent.left
        anchors.leftMargin: 40

        onClicked: root.openSources()
    }

    Button {
        width: 40
        text: "Go"
        anchors.right: parent.right
        anchors.top: parent.top

        onClicked: root.search(site, textFieldSearch.text, page)
    }
}

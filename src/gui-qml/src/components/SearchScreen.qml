import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

import "../vendor"

Page {
    id: searchTab

    signal openSources()

    property int page: 1
    property string site
    property string query
    property var results

    function load(tag) {
        if (tag) {
            textFieldSearch.text = tag
        }
        backend.search(site, textFieldSearch.text, page)
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.source: "/images/icons/menu.png"
                onClicked: drawer.open()
            }

            Label {
                text: "Search"
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }
    }

    Component {
        id: imageScreen

        ImageScreen {
            image: {
                fileUrl: ""
                tags: ""
            }

            onClosed: mainStackView.pop()
        }
    }

    ScrollView {
        anchors.bottomMargin: 40
        anchors.topMargin: 40
        anchors.fill: parent
        contentHeight: resultsLayout.contentHeight

        ColumnFlow {
            id: resultsLayout
            anchors.fill: parent
            columns: 3
            model: results

            delegate: Image {
                source: modelData.previewUrl
                fillMode: Image.PreserveAspectFit

                onHeightChanged: resultsRefresher.restart()

                MouseArea {
                    anchors.fill: parent
                    onClicked: mainStackView.push(imageScreen, { image: modelData })
                }
            }
        }

        Timer {
            id: resultsRefresher
            interval: 100
            running: false
            repeat: false

            onTriggered: resultsLayout.reEvalColumns()
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
            searchTab.load()
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
            searchTab.load()
        }
    }

    SearchField {
        id: textFieldSearch

        text: "rating:safe wallpaper"
        placeholderText: "Search..."

        onEnterPressed: searchTab.load()
    }

    Button {
        text: "Sources"
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.rightMargin: 40
        anchors.left: parent.left
        anchors.leftMargin: 40

        onClicked: searchTab.openSources()
    }

    Button {
        id: searchButton
        width: 40
        text: "Go"
        anchors.right: parent.right
        anchors.top: parent.top

        onClicked: searchTab.load()
    }
}

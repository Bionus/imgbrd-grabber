import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12
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

            SearchField {
                id: textFieldSearch

                text: ""
                placeholderText: qsTr("Search...")
                Layout.fillWidth: true
                Layout.fillHeight: true

                onEnterPressed: searchTab.load()
            }

            ToolButton {
                icon.source: "/images/icons/search.png"
                onClicked: searchTab.load()
            }
        }
    }

    Component {
        id: imageScreen

        ImageScreen {
            images: results
            index: 0

            onClosed: mainStackView.pop()
        }
    }

    ColumnLayout {
        spacing: 0
        anchors.fill: parent

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true
            contentHeight: resultsLayout.contentHeight
            clip: true

            ColumnFlow {
                id: resultsLayout
                anchors.fill: parent
                columns: gSettings.resultsColumnCount.value
                model: results

                onColumnsChanged: resultsRefresher.restart()

                delegate: Image {
                    source: modelData.previewUrl
                    fillMode: Image.PreserveAspectFit

                    onHeightChanged: resultsRefresher.restart()

                    MouseArea {
                        anchors.fill: parent
                        onClicked: mainStackView.push(imageScreen, { index: index })
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

        RowLayout {
            spacing: 0
            Layout.fillWidth: true
            Layout.fillHeight: false

            Button {
                id: prevButton
                background.anchors.fill: prevButton
                width: 40
                icon.source: "/images/icons/previous.png"
                enabled: query !== "" && page > 1
                Layout.fillHeight: true
                Material.elevation: 0

                onClicked: {
                    page--
                    searchTab.load()
                }
            }

            Button {
                id: sourcesButton
                background.anchors.fill: sourcesButton
                text: qsTr("Sources")
                Layout.fillWidth: true
                Layout.fillHeight: true
                Material.elevation: 0

                onClicked: searchTab.openSources()
            }

            Button {
                id: nextButton
                background.anchors.fill: nextButton
                width: 40
                icon.source: "/images/icons/next.png"
                enabled: query !== ""
                Layout.fillHeight: true
                Material.elevation: 0

                onClicked: {
                    page++
                    searchTab.load()
                }
            }
        }
    }
}

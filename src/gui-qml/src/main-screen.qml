import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 300
    height: 500
    title: "Grabber"

    property int page: 1
    property string site: "danbooru.donmai.us"

    TabBar {
        id: tabBar
        width: parent.width

        TabButton {
            text: "Search"
        }
        TabButton {
            text: "Log"
        }
    }

    StackLayout {
        anchors.topMargin: tabBar.height
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Item {
            ScrollView {
                anchors.bottomMargin: 40
                anchors.topMargin: 40
                anchors.fill: parent

                GridView {
                    id: results
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
                    backend.search(site, textFieldSearch.text, page)
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
                    backend.search(site, textFieldSearch.text, page)
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

                onClicked: sourcesScreen.visible = true
            }

            Button {
                id: buttonSearch
                width: 40
                text: "Go"
                anchors.right: parent.right
                anchors.top: parent.top

                onClicked: backend.search(site, textFieldSearch.text, page)
            }
        }

        Item {
            ScrollView {
                anchors.fill: parent

                Label {
                    id: log
                    anchors.fill: parent
                    text: backend.log
                }
            }
        }
    }

    SourcesScreen {
        id: sourcesScreen
        visible: false
        sources: backend.sites
        anchors.fill: parent

        onAccepted: { site = source; sourcesScreen.visible = false }
        onRejected: sourcesScreen.visible = false
    }
}

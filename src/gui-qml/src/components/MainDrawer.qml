import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0

Drawer {
    id: drawer

    signal changePage(string page)
    signal openSettings()

    property string headerTitle: ""
    property string headerSubtitle: ""
    property string headerIconSource: ""
    property size headerIconSize: Qt.size (72, 72)
    property color headerBgColorLeft: "#de6262"
    property color headerBgColorRight: "#ffb850"

    implicitWidth: Math.min(280, Math.min(parent.width, parent.height) * 0.8)
    implicitHeight: parent.height

    modal: true
    interactive: true
    position: 0
    visible: false

    ColumnLayout {
        spacing: 0
        anchors.margins: 0
        anchors.fill: parent

        ListView {
            id: listView
            z: 1
            Layout.fillWidth: true
            Layout.fillHeight: true
            ScrollIndicator.vertical: ScrollIndicator {}
            currentIndex: 1

            headerPositioning: ListView.OverlayHeader
            header: Rectangle {
                z: 2
                height: 100
                width: parent.width

                gradient: Gradient {
                    orientation: Gradient.Horizontal

                    GradientStop { position: 0; color: headerBgColorLeft }
                    GradientStop { position: 1; color: headerBgColorRight }
                }

                RowLayout {
                    spacing: 16

                    anchors {
                        fill: parent
                        centerIn: parent
                        margins: 16
                    }

                    Image {
                        source: headerIconSource
                        sourceSize: headerIconSize
                    }

                    ColumnLayout {
                        spacing: 4
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Item {
                            Layout.fillHeight: true
                        }

                        Label {
                            color: "#fff"
                            text: headerTitle
                            font.weight: Font.Medium
                            font.pixelSize: 16
                        }

                        Label {
                            color: "#fff"
                            opacity: 0.87
                            text: headerSubtitle
                            font.pixelSize: 12
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }
            }

            model: ListModel {
                ListElement {
                    page: "search"
                    name: qsTr("Search")
                    icon: "/images/icons/search.png"
                    enabled: true
                }
                ListElement {
                    page: "favorites"
                    name: qsTr("Favorites")
                    icon: "/images/icons/favorites.png"
                    enabled: true
                }
                ListElement {
                    page: "log"
                    name: qsTr("Log")
                    icon: "/images/icons/log.png"
                    enabled: true
                }
            }
            delegate: ItemDelegate {
                width: parent.width

                text: model.name
                icon.source: model.icon
                enabled: model.enabled

                onClicked: {
                    listView.currentIndex = index
                    changePage(model.page)
                    drawer.close()
                }
            }

            footerPositioning: ListView.OverlayFooter
            footer: ItemDelegate {
                id: footer
                width: parent.width

                text: qsTr("Settings")
                icon.source: "/images/icons/settings.png"

                onClicked: {
                    openSettings()
                    drawer.close()
                }

                MenuSeparator {
                    parent: footer
                    width: parent.width
                    anchors.verticalCenter: parent.top
                }
            }
        }
    }
}

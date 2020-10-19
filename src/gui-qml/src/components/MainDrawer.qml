import QtQuick 2.12
import QtQuick.Controls 2.5

Drawer {
    id: drawer

    width: Math.min(window.width, window.height) * 3 / 4
    height: window.height
    modal: true
    interactive: true
    position: 0
    visible: false

    ListView {
        id: listView
        anchors.fill: parent
        ScrollIndicator.vertical: ScrollIndicator {}

        currentIndex: 1

        headerPositioning: ListView.OverlayHeader
        header: Pane {
            id: header
            z: 2
            width: parent.width
            contentHeight: logo.height

            Image {
                id: logo
                width: parent.width
                height: 50
                source: "/images/logo.png"
                fillMode: implicitWidth > width || implicitHeight > height ? Image.PreserveAspectFit : Image.Pad
            }

            MenuSeparator {
                parent: header
                width: parent.width
                anchors.verticalCenter: parent.bottom
            }
        }

        model: ListModel {
            ListElement {
                name: qsTr("Search")
                icon: "/images/icons/search.png"
            }
            ListElement {
                name: qsTr("Favorites")
                icon: "/images/icons/favorites.png"
            }
            ListElement {
                name: qsTr("Log")
                icon: "/images/icons/log.png"
            }
        }
        delegate: ItemDelegate {
            width: parent.width

            text: model.name
            icon.source: model.icon

            onClicked: {
                listView.currentIndex = index
                // TODO: switch page (stackView.push)
                drawer.close()
            }
        }

        footerPositioning: ListView.OverlayFooter
        footer: ItemDelegate {
            id: footer
            width: parent.width

            text: qsTr("Settings")
            icon.source: "/images/icons/settings.png"

            MenuSeparator {
                parent: footer
                width: parent.width
                anchors.verticalCenter: parent.top
            }
        }
    }
}

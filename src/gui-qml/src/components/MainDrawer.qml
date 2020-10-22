import QtQuick 2.12
import QtQuick.Controls 2.5

Drawer {
    id: drawer

    signal changePage(string page)
    signal openSettings()

    implicitWidth: Math.min(280, Math.min(parent.width, parent.height) * 0.8)
    implicitHeight: parent.height

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
                page: "search"
                name: qsTr("Search")
                icon: "/images/icons/search.png"
                enabled: true
            }
            ListElement {
                page: "favorites"
                name: qsTr("Favorites")
                icon: "/images/icons/favorites.png"
                enabled: false
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

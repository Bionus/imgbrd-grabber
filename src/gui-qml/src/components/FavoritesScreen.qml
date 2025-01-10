import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Page {
    id: root

    signal openFavorite(string favorite)

    property var favorites
    property string sort: "name"
    property bool reverse: false

    property var sortedFavorites: root.favorites.concat().sort(root.sortCompare)
    function sortCompare(a, b) {
        const diff = a.localeCompare(b, undefined, {'sensitivity': 'base'});
        return root.reverse ? -diff : diff;
    }
    function sortBy(newSort) {
        if (root.sort === newSort) {
            root.reverse = !root.reverse
        } else {
            root.sort = newSort
            root.reverse = false
        }

        root.sortedFavorites = root.favorites.concat().sort(root.sortCompare)
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.source: "/images/icons/menu.png"
                onClicked: drawer.open()
            }

            Label {
                text: qsTr("Favorites")
                elide: Label.ElideRight
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }

            ToolButton {
                icon.source: "/images/icons/sort.png"
                onClicked: sortMenu.popup()
            }
        }
    }

    Menu {
        id: sortMenu
        title: qsTr("Sort")

        MenuItem {
            text: qsTr("Name")
            onTriggered: root.sortBy("name")
        }
    }

    ListView {
        anchors.fill: parent
        model: root.sortedFavorites

        delegate: ItemDelegate {
            width: root.width
            text: modelData

            onClicked: root.openFavorite(modelData)
        }
    }
}

import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Page {
    id: root

    signal openFavorite(string favorite)

    property var favorites

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
        }
    }

    ListView {
        anchors.fill: parent
        model: favorites

        delegate: ItemDelegate {
            width: parent.width
            text: modelData

            onClicked: root.openFavorite(modelData)
        }
    }
}

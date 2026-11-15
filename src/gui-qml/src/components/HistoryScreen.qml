import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Page {
    id: root

    signal openSearch(string query, string site)

    property var history

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.source: "/images/icons/menu.png"
                onClicked: drawer.open()
            }

            Label {
                text: qsTr("History")
                elide: Label.ElideRight
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }

            ToolButton {
                icon.source: "/images/icons/delete.png"
                onClicked: backend.clearHistory()
            }
        }
    }

    ListView {
        anchors.fill: parent
        model: root.history

        delegate: RowLayout {
            width: parent.width

            ItemDelegate {
                Layout.fillWidth: true

                text: modelData.query
                icon.source: backend.sites.filter(site => site.url === modelData.sites[0])[0].icon
                icon.color: "transparent"
                icon.height: 34
                icon.width: 34

                onClicked: root.openSearch(modelData.query, modelData.sites[0])
            }

            ToolButton {
                icon.source: "/images/icons/delete.png"
                Layout.fillHeight: true

                onClicked: backend.removeHistory(modelData.query, modelData.sites[0])
            }
        }
    }
}

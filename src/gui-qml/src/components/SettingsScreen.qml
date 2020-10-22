import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Page {
    id: root

    signal closed()

    property string currentSource
    property var sources

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.source: "/images/icons/back.png"
                onClicked: root.closed()
            }

            Label {
                text: "Settings"
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }
    }

    ColumnLayout {
        width: parent.width

        Text {
            Layout.fillWidth: true
            text: "Save"
            font.bold: true
        }

        SettingItem {
            Layout.fillWidth: true
            name: "Filename"
            subtitle: "%md5%.%ext%"

            onClicked: subtitle = "edited!"
        }

        SettingItem {
            Layout.fillWidth: true
            name: "Folder"
            subtitle: "/data"

            onClicked: subtitle = "edited!"
        }

        SettingItem {
            Layout.fillWidth: true
            name: "Do something"
            subtitle: "Check this to do something."

            onClicked: checkSomething.toggle()
            CheckBox {
                id: checkSomething
            }
        }
    }
}

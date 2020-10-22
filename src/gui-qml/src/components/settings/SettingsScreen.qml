import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.12
import Qt.labs.platform 1.1

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

        TextFieldSetting {
            id: filenameSetting

            name: "Filename"
            value: "%md5%.%ext%"
            Layout.fillWidth: true

            onChanged: filenameSetting.value = value
        }

        FolderSetting {
            id: folderSetting

            name: "Folder"
            value: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
            Layout.fillWidth: true

            onChanged: folderSetting.value = value
        }

        CheckBoxSetting {
            name: "Do something"
            subtitle: "Check this to do something."
            Layout.fillWidth: true
        }
    }
}

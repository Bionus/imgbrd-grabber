import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import Qt.labs.platform 1.1

Page {
    id: root

    signal closed()

    property string currentSource
    property var sources

    padding: 8

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.source: "/images/icons/back.png"
                onClicked: root.closed()
            }

            Label {
                text: qsTr("Settings")
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
            text: qsTr("Interface")
            font.bold: true
        }

        ComboSetting {
            name: qsTr("Language")
            options: ["English", "French"]
            settingKey: "language"
            settingDefault: "English"
            Layout.fillWidth: true

            onChanged: languageLoader.setLanguage(value)
        }

        Text {
            Layout.fillWidth: true
            text: qsTr("Save")
            font.bold: true
        }

        TextFieldSetting {
            name: qsTr("Filename")
            settingKey: "Save/filename"
            settingDefault: "%md5%.%ext%"
            Layout.fillWidth: true
        }

        FolderSetting {
            name: qsTr("Folder")
            settingKey: "Save/path"
            settingDefault: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
            Layout.fillWidth: true
        }

        CheckBoxSetting {
            name: qsTr("Hide blacklisted")
            subtitle: qsTr("Hide blacklisted images from the results.")
            settingKey: "hideblacklisted"
            settingDefault: true
            Layout.fillWidth: true
        }
    }
}

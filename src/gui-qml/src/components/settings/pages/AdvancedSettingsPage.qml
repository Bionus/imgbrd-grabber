import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs
import Qt.labs.platform 1.1 as Labs

import "../items"

ColumnLayout {
    spacing: 0

    SettingTitle {
        Layout.fillWidth: true
        text: qsTr("Updates")
    }
    ComboSetting {
        name: qsTr("Check for updates interval")
        options: [qsTr("Every time"), qsTr("Once a day"), qsTr("Once a week"), qsTr("Once a month"), qsTr("Never")]
        values: [0, 24 * 60 * 60, 7 * 24 * 60 * 60, 30 * 24 * 60 * 60, -1]
        setting: gSettings.checkForUpdates
        Layout.fillWidth: true
    }

    SettingTitle {
        Layout.fillWidth: true
        text: qsTr("Backup")
    }
    SettingItem {
        name: qsTr("Export settings")
        subtitle: qsTr("Backup the app settings.ini file on your device.")
        Layout.fillWidth: true

        onClicked: exportDialog.open()

        FileDialog {
            id: exportDialog

            title: qsTr("Please choose a directory")
            folder: Labs.StandardPaths.standardLocations(Labs.StandardPaths.DownloadLocation)[0]
            selectFolder: true

            onAccepted: backend.exportSettings(backend.toLocalFile(exportDialog.fileUrl.toString() + "/settings.ini"))
        }
    }
    SettingItem {
        name: qsTr("Import settings")
        subtitle: qsTr("Import the app settings.ini from an existing file.")
        Layout.fillWidth: true

        onClicked: importDialog.open()

        FileDialog {
            id: importDialog

            title: qsTr("Please choose a file")
            folder: Labs.StandardPaths.standardLocations(Labs.StandardPaths.DownloadLocation)[0]

            onAccepted: backend.importSettings(backend.toLocalFile(importDialog.fileUrl.toString()))
        }
    }

    Item {
        Layout.fillHeight: true
    }
}

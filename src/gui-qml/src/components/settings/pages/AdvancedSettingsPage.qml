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
        text: qsTr("Auto-complete")
    }
    SpinBoxSetting {
        name: qsTr("Auto tag add")
        min: 0
        max: 100000
        setting: gSettings.tagsAutoAdd
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

        FolderDialog {
            id: exportDialog

            title: qsTr("Please choose a directory")
            currentFolder: Labs.StandardPaths.standardLocations(Labs.StandardPaths.DownloadLocation)[0]

            onAccepted: backend.exportSettings(backend.toLocalFile(exportDialog.selectedFolder.toString() + "/settings.ini"))
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
            currentFolder: Labs.StandardPaths.standardLocations(Labs.StandardPaths.DownloadLocation)[0]

            onAccepted: backend.importSettings(backend.toLocalFile(importDialog.fileUrl.toString()))
        }
    }
    SettingItem {
        name: qsTr("Export backup")
        subtitle: qsTr("Backup all setting files as a ZIP on your device.")
        Layout.fillWidth: true

        onClicked: exportBackupDialog.open()

        FolderDialog {
            id: exportBackupDialog

            title: qsTr("Please choose a directory")
            currentFolder: Labs.StandardPaths.standardLocations(Labs.StandardPaths.DownloadLocation)[0]

            onAccepted: {
                const datePart = new Date().toISOString()
                    .replace(/[:-]/g, '.')
                    .replace('T', '-')
                    .replace('Z', '')
                const filename = "backup-" + datePart + ".zip"
                backend.exportBackup(backend.toLocalFile(exportBackupDialog.selectedFolder.toString() + "/" + filename))
            }
        }
    }
    SettingItem {
        enabled: false
        name: qsTr("Import backup")
        subtitle: qsTr("Import all app settings from an existing ZIP file.")
        Layout.fillWidth: true

        onClicked: importBackupDialog.open()

        FileDialog {
            id: importBackupDialog

            title: qsTr("Please choose a file")
            currentFolder: Labs.StandardPaths.standardLocations(Labs.StandardPaths.DownloadLocation)[0]

            onAccepted: backend.importBackup(backend.toLocalFile(importBackupDialog.fileUrl.toString()))
        }
    }

    Item {
        Layout.fillHeight: true
    }
}

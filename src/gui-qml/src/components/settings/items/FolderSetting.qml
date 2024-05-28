import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs

import ".."

Item {
    id: root

    property string name
    property Setting setting

    implicitHeight: item.implicitHeight

    SettingItem {
        id: item

        name: root.name
        subtitle: setting.value.startsWith("/storage/emulated/0/")
            ? setting.value.substr(19)
            : setting.value
        anchors.fill: parent

        onClicked: dialog.open()

        FolderDialog {
            id: dialog

            title: qsTr("Please choose a directory")
            selectedFolder: setting.value

            onAccepted: setting.setValue(backend.toLocalFile(dialog.selectedFolder.toString()))
        }
    }
}

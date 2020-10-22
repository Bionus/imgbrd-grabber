import QtQuick 2.12
import QtQuick.Controls 2.5
import Qt.labs.platform 1.1

Item {
    id: setting

    signal changed(string value)

    property string name
    property string value

    implicitHeight: item.implicitHeight

    SettingItem {
        id: item

        name: setting.name
        subtitle: setting.value
        anchors.fill: parent

        onClicked: dialog.open()

        FolderDialog {
            id: dialog

            acceptLabel: "Select"
            folder: setting.value

            onAccepted: setting.changed(folder)
        }
    }
}

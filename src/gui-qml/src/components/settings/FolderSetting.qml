import QtQuick 2.12
import QtQuick.Controls 2.5
import Qt.labs.platform 1.1

Item {
    id: root

    property string name
    property alias settingKey: setting.settingKey
    property alias settingDefault: setting.settingDefault

    implicitHeight: item.implicitHeight

    Setting {
        id: setting
    }

    SettingItem {
        id: item

        name: root.name
        subtitle: setting.value
        anchors.fill: parent

        onClicked: dialog.open()

        FolderDialog {
            id: dialog

            acceptLabel: "Select"
            folder: "file:///" + setting.value

            onAccepted: setting.setValue(folder.toString().substring(0, 8) === "file:///" ? folder.toString().substring(8) : folder.toString())
        }
    }
}

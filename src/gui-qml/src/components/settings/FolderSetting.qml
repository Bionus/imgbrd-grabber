import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.3

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

        FileDialog {
            id: dialog

            // acceptLabel: "Select"
            title: "Please choose a directory"
            folder: "file:///" + setting.value
            // selectExisting: true
            selectFolder: true
            // selectMultiple: false

            onAccepted: setting.setValue(dialog.fileUrl.toString().substring(0, 8) === "file:///" ? dialog.fileUrl.toString().substring(8) : dialog.fileUrl.toString())
        }
    }
}

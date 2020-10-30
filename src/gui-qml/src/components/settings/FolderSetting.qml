import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.3

Item {
    id: root

    property string name
    property Setting setting

    implicitHeight: item.implicitHeight

    SettingItem {
        id: item

        name: root.name
        subtitle: backend.toLocalFile(setting.value)
        anchors.fill: parent

        onClicked: dialog.open()

        FileDialog {
            id: dialog

            title: qsTr("Please choose a directory")
            folder: setting.value
            selectFolder: true

            onAccepted: setting.setValue(dialog.fileUrl.toString())
        }
    }
}

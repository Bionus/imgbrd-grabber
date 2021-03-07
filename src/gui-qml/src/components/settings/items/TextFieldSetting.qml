import QtQuick 2.12
import QtQuick.Controls 2.5

import ".."

Item {
    id: root

    property string name
    property Setting setting
    property alias echoMode: textField.echoMode
    property alias inputMethodHints: textField.inputMethodHints

    implicitHeight: item.implicitHeight

    SettingItem {
        id: item

        name: root.name
        subtitle: textField.echoMode === TextField.Password
            ? "*".repeat(setting.value.length)
            : (setting.value.length > 0 && setting.value.trim().length === 0
                ? '"' + setting.value + '"'
                : setting.value)
        anchors.fill: parent

        onClicked: dialog.open()

        Dialog {
            id: dialog

            title: root.name
            anchors.centerIn: Overlay.overlay
            modal: true
            standardButtons: Dialog.Ok | Dialog.Cancel

            onAccepted: setting.setValue(textField.text)
            onRejected: textField.text = setting.value

            TextField {
                id: textField
                anchors.fill: parent
                text: setting.value
            }
        }
    }
}

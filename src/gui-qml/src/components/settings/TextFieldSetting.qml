import QtQuick 2.12
import QtQuick.Controls 2.5

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

        Dialog {
            id: dialog

            title: name
            anchors.centerIn: Overlay.overlay
            modal: true
            standardButtons: Dialog.Ok | Dialog.Cancel

            onAccepted: setting.changed(textField.text)
            onRejected: textField.text = value

            TextField {
                id: textField
                text: value
            }
        }
    }
}

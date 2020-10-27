import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    id: root

    property string name
    property int min: 0
    property int max: 99
    property int step: 1
    property alias settingKey: setting.settingKey
    property alias settingDefault: setting.settingDefault
    property alias settingObject: setting.settingObject

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

        Dialog {
            id: dialog

            title: root.name
            anchors.centerIn: Overlay.overlay
            modal: true
            standardButtons: Dialog.Ok | Dialog.Cancel

            onAccepted: setting.setValue(spinBox.value)
            onRejected: spinBox.value = setting.value

            SpinBox {
                id: spinBox
                value: setting.value
                from: root.min
                to: root.max
                stepSize: root.step
            }
        }
    }
}

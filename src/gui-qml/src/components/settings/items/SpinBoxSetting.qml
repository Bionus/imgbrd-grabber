import QtQuick 2.12
import QtQuick.Controls 2.5

import ".."

Item {
    id: root

    property string name
    property int min: 0
    property int max: 99
    property int step: 1
    property Setting setting

    implicitHeight: item.implicitHeight

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
                anchors.fill: parent
                value: setting.value
                from: root.min
                to: root.max
                stepSize: root.step
            }
        }
    }
}

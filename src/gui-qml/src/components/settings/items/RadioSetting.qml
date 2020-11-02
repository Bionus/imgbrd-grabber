import QtQuick 2.12
import QtQuick.Controls 2.5

import ".."
import "../.."

Item {
    id: root

    signal changed(string value)

    property string name
    property var options
    property var values: options
    property Setting setting
    property var currentValue: setting.value

    implicitHeight: item.implicitHeight

    SettingItem {
        id: item

        name: root.name
        subtitle: options[values.indexOf(setting.value)] || ""
        anchors.fill: parent

        onClicked: dialog.open()

        Dialog {
            id: dialog

            title: root.name
            anchors.centerIn: Overlay.overlay
            modal: true
            standardButtons: Dialog.Ok | Dialog.Cancel

            onAccepted: {
                var val = currentValue
                setting.setValue(val)
                root.changed(val)
            }
            onRejected: currentValue = setting.value

            height: root.options.length * 30 + 130 // TODO: do this properly

            ButtonGroup {
                id: buttonGroup
            }

            ListView {
                anchors.fill: parent
                model: root.options
                height: root.options.length * 30

                delegate: RadioDelegate {
                    width: parent.width
                    checked: values[index] === currentValue
                    text: modelData
                    ButtonGroup.group: buttonGroup
                    height: 30 // from 40
                    padding: 8 // from 12

                    indicator: MiniRadioIndicator {
                        control: parent
                        size: 18 // from 28
                    }

                    onCheckedChanged: if (checked) currentValue = values[index]
                }
            }
        }
    }
}

import QtQuick 2.12
import QtQuick.Controls 2.5

import ".."

Item {
    id: root

    property string name
    property var options
    property var values: options
    property Setting setting

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

            onAccepted: setting.setValue(values[comboBox.currentIndex])
            onRejected: comboBox.currentIndex = values.indexOf(setting.value)

            ComboBox {
                id: comboBox
                anchors.fill: parent

                textRole: "text"
                valueRole: "value"
                model: ListModel {
                    Component.onCompleted: {
                        for (var i = 0; i < values.length; ++i) {
                            append({ text: options[i], value: values[i] });
                        }
                    }
                }

                Component.onCompleted: {
                    currentIndex = comboBox.currentIndex = values.indexOf(setting.value)
                }
            }
        }
    }
}

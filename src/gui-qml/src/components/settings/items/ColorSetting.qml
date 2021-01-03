import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls.Material 2.12

import ".."

Item {
    id: root

    property string name
    property Setting setting
    property var colors: globals.materialColors.map(c => Material.color(c, Material.Shade700))
    property var values: globals.materialColors.map((v, i) => i)
    property var labels: globals.materialColorsStr
    property bool showLabels: false
    property var currentValue: setting.value

    implicitHeight: item.implicitHeight

    SettingItem {
        id: item

        name: root.name
        subtitle: labels ? labels[values ? values.indexOf(setting.value) : colors.indexOf(setting.value)] : setting.value
        anchors.fill: parent

        onClicked: dialog.open()

        Dialog {
            id: dialog

            title: root.name
            anchors.centerIn: Overlay.overlay
            modal: true
            standardButtons: Dialog.Ok | Dialog.Cancel

            onAccepted: setting.setValue(currentValue)
            onRejected: currentValue = setting.value

            GridLayout {
                anchors.fill: parent
                columns: 4

                Repeater {
                    model: root.colors

                    Rectangle {
                        color: modelData
                        width: 60
                        height: 60
                        radius: width * 0.2
                        border.color: "#000000"
                        border.width: currentValue === (values ? values[index] : modelData) ? 3 : 0

                        Label {
                            anchors.fill: parent
                            text: labels && labels[index]
                            horizontalAlignment: Label.AlignHCenter
                            verticalAlignment: Label.AlignVCenter
                            visible: showLabels && labels && labels[index]
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                currentValue = values ? values[index] : modelData
                                dialog.accept()
                            }
                        }
                    }
                }
            }
        }
    }
}

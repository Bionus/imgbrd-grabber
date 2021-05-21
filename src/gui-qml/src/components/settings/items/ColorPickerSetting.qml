import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls.Material 2.12

import ".."

Item {
    id: root

    property string name
    property Setting setting
    property bool allowAuto: false
    property var currentValue: setting.value
    property color initialValueColor: setting.value || "black"
    property color currentValueColor: setting.value || "black"

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

            onAccepted: setting.setValue(currentValue)
            onRejected: currentValue = setting.value

            ColumnLayout {
                anchors.fill: parent

                Button {
                    text: "Automatic"
                    visible: allowAuto
                    Layout.fillWidth: true

                    onClicked: {
                        currentValue = ""
                        dialog.accept()
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 60
                    color: currentValueColor
                }

                Text {
                    text: currentValue
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                }

                RowLayout {
                    Text {
                        text: "R"
                    }

                    Slider {
                        id: redSlider

                        from: 0
                        to: 255
                        stepSize: 1
                        value: Math.round(initialValueColor.r * 255)

                        onValueChanged: {
                            currentValueColor.r = value / 255
                            currentValue = currentValueColor
                        }
                    }

                    Text {
                        text: Math.round(redSlider.value)
                    }
                }

                RowLayout {
                    Text {
                        text: "G"
                    }

                    Slider {
                        id: greenSlider

                        from: 0
                        to: 255
                        stepSize: 1
                        value: Math.round(initialValueColor.g * 255)

                        onValueChanged: {
                            currentValueColor.g = value / 255
                            currentValue = currentValueColor
                        }
                    }

                    Text {
                        text: Math.round(greenSlider.value)
                    }
                }

                RowLayout {
                    Text {
                        text: "B"
                    }

                    Slider {
                        id: blueSlider

                        from: 0
                        to: 255
                        stepSize: 1
                        value: Math.round(initialValueColor.b * 255)

                        onValueChanged: {
                            currentValueColor.b = value / 255
                            currentValue = currentValueColor
                        }
                    }

                    Text {
                        text: Math.round(blueSlider.value)
                    }
                }
            }
        }
    }
}

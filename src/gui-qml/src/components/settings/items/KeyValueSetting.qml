import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

ColumnLayout {
    id: root

    property var values
    property int editIndex: 0

    signal append(string key, string value)
    signal edit(int index, string key, string value, string oldKey, string oldValue)
    signal remove(int index, string key)

    Layout.leftMargin: 16
    Layout.rightMargin: 16

    ColumnLayout {
        spacing: 0

        Repeater {
            id: listView
            Layout.fillWidth: true

            model: values

            delegate: RowLayout {
                width: listView.width

                Label {
                    text: modelData.key
                    font.bold: true
                }
                Label {
                    text: modelData.value
                    Layout.fillWidth: true
                }

                ToolButton {
                    icon.source: "/images/icons/edit.png"
                    Layout.fillHeight: true

                    onClicked: {
                        editIndex = index
                        editTextFieldKey.text = modelData.key
                        editTextFieldValue.text = modelData.value
                        editDialog.open()
                    }
                }

                ToolButton {
                    icon.source: "/images/icons/remove.png"
                    Layout.fillHeight: true

                    onClicked: remove(index, values[index].key)
                }
            }
        }
    }

    Button {
        icon.source: "/images/icons/add.png"
        text: qsTr("Add")
        Layout.fillWidth: true

        onClicked: {
            addTextFieldKey.text = ""
            addTextFieldValue.text = ""
            addDialog.open()
        }
    }

    Dialog {
        id: addDialog

        title: qsTr("Add")
        anchors.centerIn: Overlay.overlay
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: append(addTextFieldKey.text, addTextFieldValue.text)

        ColumnLayout {
            Label {
                text: qsTr("Key")
            }
            TextField {
                id: addTextFieldKey
                width: parent.width
            }

            Label {
                text: qsTr("Value")
            }
            TextField {
                id: addTextFieldValue
                width: parent.width
            }
        }
    }

    Dialog {
        id: editDialog

        title: qsTr("Edit")
        anchors.centerIn: Overlay.overlay
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: edit(editIndex, editTextFieldKey.text, editTextFieldValue.text, values[editIndex].key, values[editIndex].value)

        ColumnLayout {
            Label {
                text: qsTr("Key")
            }
            TextField {
                id: editTextFieldKey
                width: parent.width
            }

            Label {
                text: qsTr("Value")
            }
            TextField {
                id: editTextFieldValue
                width: parent.width
            }
        }
    }
}

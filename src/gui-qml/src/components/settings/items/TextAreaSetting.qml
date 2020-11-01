import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    id: root

    property string name
    property string subtitle
    property var setting

    implicitHeight: item.implicitHeight

    SettingItem {
        id: item

        name: root.name
        subtitle: root.subtitle
        anchors.fill: parent

        onClicked: dialog.open()

        Dialog {
            id: dialog

            title: root.name
            anchors.centerIn: Overlay.overlay
            modal: true
            standardButtons: Dialog.Ok | Dialog.Cancel
            height: 300

            onAccepted: setting.setValue(textArea.text)
            onRejected: textArea.text = setting.value

            ScrollView {
                anchors.fill: parent

                TextArea {
                    id: textArea
                    text: setting.value
                }
            }
        }
    }
}

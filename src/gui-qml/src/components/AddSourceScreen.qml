import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.12

Page {
    id: root

    signal accepted()
    signal rejected()

    padding: 8

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.source: "/images/icons/back.png"
                onClicked: root.rejected()
            }

            Label {
                text: "Add new source"
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }
    }

    MessageDialog {
        id: errorDialog
        title: "Error"
        icon: StandardIcon.Critical
    }

    ColumnLayout {
        anchors.fill: parent

        Text {
            Layout.fillWidth: true
            text: "Type"
        }
        ComboBox {
            id: typeField
            Layout.fillWidth: true
            model: backend.sources
        }

        Text {
            Layout.fillWidth: true
            text: "URL"
        }
        TextField {
            id: urlField
            Layout.fillWidth: true
        }

        CheckBox {
            id: httpsField
            Layout.fillWidth: true
            text: "HTTPS"
            checked: true
        }

        Item {
            Layout.fillHeight: true
        }
    }

    footer: Button {
        Layout.fillWidth: true
        text: "Add"

        onClicked: {
            var result = backend.addSite(typeField.currentText, urlField.text, httpsField.checked)
            if (result !== "") {
                errorDialog.text = result
                errorDialog.open()
            } else {
                root.accepted()
            }
        }
    }
}

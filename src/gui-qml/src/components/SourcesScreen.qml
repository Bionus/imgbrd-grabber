import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

ColumnLayout {
    id: root

    signal accepted(string source)
    signal rejected()

    property string currentSource
    property var sources

    ButtonGroup {
        id: buttonGroup
    }

    ListView {
        id: results
        Layout.fillWidth: true
        Layout.fillHeight: true
        model: sources

        delegate: RadioDelegate {
            width: parent.width
            checked: modelData == currentSource
            text: modelData
            ButtonGroup.group: buttonGroup
            height: 30 // from 40
            padding: 8 // from 12

            indicator: MiniRadioIndicator {
                control: parent
                size: 18 // from 28
            }

            onCheckedChanged: if (checked) currentSource = modelData
        }
    }

    DialogButtonBox {
        standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
        Layout.fillWidth: true

        onAccepted: root.accepted(buttonGroup.checkedButton.text)
        onRejected: root.rejected()
    }
}

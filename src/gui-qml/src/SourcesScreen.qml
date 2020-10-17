import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

ColumnLayout {
    id: root

    signal accepted(string source)
    signal rejected()
    property var sources

    anchors.fill: parent

    ListView {
        id: results
        Layout.fillWidth: true
        Layout.fillHeight: true
        model: sources

        delegate: Text {
            width: parent.width
            height: 50
            text: modelData
        }
    }

    DialogButtonBox {
        standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
        Layout.fillWidth: true

        onAccepted: root.accepted(sources[0]) // TODO: add ComboBox / CheckBox selection
        onRejected: root.rejected()
    }
}

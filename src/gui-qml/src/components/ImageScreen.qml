import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

ColumnLayout {
    id: root

    signal closed()
    property var fileUrl

    Image {
        Layout.fillWidth: true
        Layout.fillHeight: true
        source: fileUrl
        fillMode: Image.PreserveAspectFit
    }

    DialogButtonBox {
        standardButtons: DialogButtonBox.Close
        Layout.fillWidth: true

        onRejected: root.closed()
    }
}

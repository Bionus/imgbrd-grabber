import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

ColumnLayout {
    id: root

    signal closed()
    property var fileUrl

    Image {
        id: image
        Layout.fillWidth: true
        Layout.fillHeight: true
        source: fileUrl
        fillMode: Image.PreserveAspectFit
    }

    ProgressBar {
        value: image.progress
        visible: image.status != Image.Ready
        Layout.fillWidth: true
    }

    DialogButtonBox {
        standardButtons: DialogButtonBox.Close
        Layout.fillWidth: true

        onRejected: root.closed()
    }
}

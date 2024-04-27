import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12

Rectangle {
    color: Material.theme === Material.Dark
        ? Qt.rgba(0, 0, 0, 0.5)
        : Qt.rgba(1, 1, 1, 0.5)

    BusyIndicator {
        anchors.centerIn: parent
    }
}

import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs
import QtQuick.Layouts 1.12

Item {
    id: root

    property color color
    property int size: 1

    anchors.fill: parent

    // Top
    Rectangle  {
        anchors.top: parent.top
        width: parent.width
        color: root.color
        height: 3
    }

    // Bottom
    Rectangle  {
        anchors.bottom: parent.bottom
        width: parent.width
        color: root.color
        height: 3
    }

    // Left
    Rectangle  {
        anchors.left: parent.left
        height: parent.height
        color: root.color
        width: 3
    }

    // Right
    Rectangle  {
        anchors.right: parent.right
        height: parent.height
        color: root.color
        width: 3
    }
}

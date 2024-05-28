import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs
import QtQuick.Layouts 1.12

Rectangle {
    id: root

    property alias text: label.text

    anchors.right: parent.right
    anchors.top: parent.top
    anchors.margins: 4
    color: "#ec3e3a"
    radius: width / 2
    height: 22
    width: label.text.length > 2 ? label.paintedWidth + height / 2 : height

    Label {
        id: label

        color: "#fdfdfdfd"
        font.pixelSize: 12
        anchors.fill: parent
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.margins: parent.anchors.margins
    }
}

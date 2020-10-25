import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.impl 2.5
import QtQuick.Layouts 1.12

Rectangle {
    id: root

    signal clicked()

    default property alias contents: placeholder.data
    property string name
    property string subtitle

    implicitHeight: Math.max(placeholder.implicitHeight, colLayout.implicitHeight) + rowLayout.anchors.topMargin + rowLayout.anchors.bottomMargin
    color: mouseArea.pressed ? Qt.rgba(0, 0, 0, 0.1) : "transparent"

    RowLayout {
        id: rowLayout
        anchors.fill: parent
        anchors.topMargin: 6
        anchors.bottomMargin: 6
        anchors.leftMargin: 8
        anchors.rightMargin: 8

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                id: colLayout
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width
                spacing: 2

                Text {
                    text: name
                }
                Text {
                    visible: !!subtitle
                    text: subtitle
                    font.italic: true
                }
            }
        }

        Item {
            id: placeholder
            implicitWidth: childrenRect.width
            implicitHeight: childrenRect.height
            Layout.fillHeight: true
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: root.clicked()
    }
}

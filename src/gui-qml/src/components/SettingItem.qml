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

    implicitHeight: Math.max(placeholder.implicitHeight, colLayout.implicitHeight)

    SystemPalette { id: palette }
    color: Color.blend(palette.button, palette.mid, mouseArea.pressed ? 0.5 : 0.0)

    RowLayout {
        id: rowLayout
        anchors.fill: parent

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                id: colLayout
                anchors.fill: parent
                spacing: 0

                Text {
                    text: name
                }
                Text {
                    visible: !!subtitle
                    text: subtitle
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

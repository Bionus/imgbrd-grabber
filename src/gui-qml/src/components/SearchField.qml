import Grabber 1.0
import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.5

FocusScope {
    id: root

    signal enterPressed()
    property alias text: textInput.text
    property alias placeholderText: placeholder.text

    anchors.right: parent.right
    anchors.rightMargin: 40
    anchors.left: parent.left
    anchors.top: parent.top
    activeFocusOnTab: true
    implicitHeight: 40
    implicitWidth: Math.round(parent.width)

    Rectangle {
        anchors.fill: editbg
        radius: editbg.radius
        color: "#aaffffff"
        anchors.bottomMargin: -1
    }

    Rectangle {
        id: editbg
        anchors.fill: parent
        border.width: 1
        border.color: "#ccc"

        TextEdit {
            id: textInput

            anchors.fill: parent
            clip: true
            anchors.leftMargin: 16
            anchors.rightMargin: 16
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 13
            focus: true

            Keys.onEnterPressed: {
                root.enterPressed()
                event.accepted = true
            }
            Keys.onReturnPressed: {
                root.enterPressed()
                event.accepted = true
            }

            Text {
                id: placeholder

                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                visible: !(parent.text.length || textInput.inputMethodComposing)
                font: parent.font
                color: "#aaa"
            }
        }

        SyntaxHighlighterHelper {
            quickDocument: textInput.textDocument
        }
    }
}

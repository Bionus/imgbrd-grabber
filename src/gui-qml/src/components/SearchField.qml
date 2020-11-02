import Grabber 1.0
import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.5

FocusScope {
    id: root

    signal enterPressed()
    property alias text: textInput.text
    property alias placeholderText: placeholder.text

    activeFocusOnTab: true

    Rectangle {
        id: editbg
        anchors.fill: parent
        anchors.topMargin: 8
        anchors.bottomMargin: 8
        color: Qt.rgba(1, 1, 1, 0.6)
        radius: 12

        TextEdit {
            id: textInput

            anchors.fill: parent
            clip: true
            anchors.leftMargin: 12
            anchors.rightMargin: 12
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 14
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
                color: "#666"
            }
        }

        SyntaxHighlighterHelper {
            quickDocument: textInput.textDocument
        }
    }
}

import Grabber 1.0
import QtQuick 2.12
import QtQuick.Controls 2.5

TextEdit {
    id: root

    signal enterPressed()
    property alias placeholderText: placeholder.text

    verticalAlignment: Text.AlignVCenter
    font.pixelSize: 14

    Keys.onEnterPressed: {
        Qt.inputMethod.hide()
        root.enterPressed()
        event.accepted = true
    }
    Keys.onReturnPressed: {
        Qt.inputMethod.hide()
        root.enterPressed()
        event.accepted = true
    }

    Text {
        id: placeholder

        anchors.fill: parent
        verticalAlignment: Text.AlignVCenter
        visible: !(parent.text.length || parent.inputMethodComposing)
        font: parent.font
        color: "#666"
    }

    SyntaxHighlighterHelper {
        quickDocument: root.textDocument
    }
}

import Grabber 1.0
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12

SearchEdit {
    id: root

    property bool blockTextChange: true

    inputMethodHints: Qt.ImhSensitiveData | Qt.ImhPreferLowercase
    Component.onCompleted: blockTextChange = false

    onTextChanged: {
        if (blockTextChange) {
            blockTextChange = false
            return
        }

        var currentWord = getCurrentWord()
        backend.loadSuggestions(currentWord[0], 10)
        completions.currentIndex = -1
        completionsBox.visible = completions.model.length > 0
    }

    onActiveFocusChanged: {
        if (!activeFocus) {
            completionsBox.visible = false
        }
    }

    function setText(newText) {
        root.blockTextChange = true
        root.text = newText
    }
    function getCurrentWord() {
        var begin = root.text.lastIndexOf(" ", root.cursorPosition - 1) + 1
        var end = root.text.indexOf(" ", begin);
        if (end < 0) {
            end = root.text.length;
        }
        return [root.text.substring(begin, end), begin, end];
    }
    function highlight(suggestion) {
        var currentWord = getCurrentWord()
        var lastWord = currentWord[0]
        return "<b>" + suggestion.substring(0, lastWord.length) + "</b>" + suggestion.substring(lastWord.length)
    }

    Rectangle {
        id: completionsBox

        visible: false
        anchors.top: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: childrenRect.height
        color: Material.backgroundDimColor

        Column {
            Repeater {
                id: completions

                property int currentIndex: -1

                model: backend.autoComplete
                delegate: ItemDelegate {
                    id: listItem

                    text: highlight(modelData)
                    width: completionsBox.width
                    height: 30
                    highlighted: index == completions.currentIndex

                    onClicked: {
                        var currentWord = getCurrentWord()
                        var left = root.text.substring(0, currentWord[1])
                        var right = root.text.substring(currentWord[2])
                        var newText = left + modelData + right

                        completions.currentIndex = index
                        root.blockTextChange = true
                        root.text = newText
                        root.cursorPosition = (left + modelData).length
                        completionsBox.visible = false
                    }
                }
            }
        }
    }
}

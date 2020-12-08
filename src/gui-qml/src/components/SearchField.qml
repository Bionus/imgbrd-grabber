import Grabber 1.0
import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.5

FocusScope {
    id: root

    signal enterPressed()
    property alias text: textInput.text
    property alias placeholderText: textInput.placeholderText
    property bool isOpen: false

    activeFocusOnTab: true

    Rectangle {
        id: editbg
        anchors.fill: parent
        anchors.topMargin: 8
        anchors.bottomMargin: 8
        color: Qt.rgba(1, 1, 1, 0.6)
        radius: 12

        AutoCompleteSearchEdit {
            id: textInput

            anchors.fill: parent
            anchors.leftMargin: 12
            anchors.rightMargin: 12
            focus: true

            onEnterPressed: root.enterPressed()
        }

        ToolButton {
            icon.source: root.isOpen
                ? "/images/icons/remove.png"
                : "/images/icons/add.png"
            onClicked: root.isOpen = !root.isOpen
            flat: true
            height: 34
            width: 34
            anchors.right: editbg.right
            anchors.verticalCenter: editbg.verticalCenter
        }
    }
}

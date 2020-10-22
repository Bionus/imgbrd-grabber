import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Page {
    id: root

    signal accepted(string source)
    signal rejected()
    signal addSource()

    property string currentSource
    property var sources

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.source: "/images/icons/back.png"
                onClicked: root.accepted(buttonGroup.checkedButton.text)
            }

            Label {
                text: "Sources selection"
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }

            ToolButton {
                icon.source: "/images/icons/add.png"
                onClicked: root.addSource()
            }
        }
    }

    ButtonGroup {
        id: buttonGroup
    }

    ListView {
        id: results
        anchors.fill: parent
        model: sources

        delegate: RadioDelegate {
            width: parent.width
            checked: modelData == currentSource
            text: modelData
            ButtonGroup.group: buttonGroup
            height: 30 // from 40
            padding: 8 // from 12

            indicator: MiniRadioIndicator {
                control: parent
                size: 18 // from 28
            }

            onCheckedChanged: if (checked) currentSource = modelData
        }
    }
}

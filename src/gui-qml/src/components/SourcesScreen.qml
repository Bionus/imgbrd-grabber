import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Page {
    id: root

    signal accepted(string source)
    signal rejected()
    signal addSource()
    signal editSource(var source)

    property string currentSource
    property var sources

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.source: "/images/icons/back.png"
                onClicked: root.accepted(buttonGroup.checkedButton.url)
            }

            Label {
                text: qsTr("Sources selection")
                elide: Label.ElideRight
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

        delegate: RowLayout {
            width: parent.width

            RadioButton {
                property string url: modelData.url

                width: parent.width
                checked: modelData.url === currentSource
                text: modelData.name || modelData.url
                ButtonGroup.group: buttonGroup
                height: 30 // from 40
                padding: 8 // from 12
                Layout.fillHeight: true
                Layout.fillWidth: true

                indicator: MiniRadioIndicator {
                    control: parent
                    size: 18 // from 28
                }

                onCheckedChanged: if (checked) currentSource = modelData.url
            }

            ToolButton {
                icon.source: "/images/icons/edit.png"
                Layout.fillHeight: true

                onClicked: root.editSource(modelData)
            }
        }
    }

    Keys.onReleased: {
        if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
            root.rejected()
            event.accepted = true
        }
    }
}

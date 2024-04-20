import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Page {
    id: root

    signal back()
    signal addSource()
    signal editSource(var source)

    property string activeSource
    property var sources

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                visible: mainStackView.depth > 1
                icon.source: "/images/icons/back.png"
                onClicked: root.back()
            }
            ToolButton {
                visible: mainStackView.depth <= 1
                icon.source: "/images/icons/menu.png"
                onClicked: drawer.open()
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
                checked: modelData.url === activeSource
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

                onCheckedChanged: if (checked) activeSource = modelData.url
            }

            ToolButton {
                icon.source: "/images/icons/edit.png"
                Layout.fillHeight: true

                onClicked: root.editSource(modelData)
            }
        }
    }
}

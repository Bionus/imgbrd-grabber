import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import "./items"
import "./pages"

Page {
    id: root

    signal closed()

    property string currentSource
    property var sources

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.source: "/images/icons/back.png"
                onClicked: stackView.depth > 1 ? stackView.pop() : root.closed()
            }

            Label {
                text: qsTr("Settings")
                elide: Label.ElideRight
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: mainSettings

        ListView {
            id: mainSettings

            model: ListModel {
                ListElement {
                    name: qsTr("Interface")
                    icon: "/images/icons/interface.png"
                }
                ListElement {
                    name: qsTr("Appearance")
                    icon: "/images/icons/palette.png"
                }
                ListElement {
                    name: qsTr("Save")
                    icon: "/images/icons/download.png"
                }
                ListElement {
                    name: qsTr("Sources")
                    icon: "/images/icons/cloud.png"
                }
                ListElement {
                    name: qsTr("Blacklist")
                    icon: "/images/icons/block.png"
                }
                ListElement {
                    name: qsTr("Network")
                    icon: "/images/icons/network.png"
                }
                ListElement {
                    name: qsTr("About")
                    icon: "/images/icons/info.png"
                }
            }
            delegate: ItemDelegate {
                width: parent.width

                text: model.name
                icon.source: model.icon

                onClicked: stackView.push(stackView.children[index + 1])
            }
        }

        InterfaceSettingsPage {
            width: parent.width
            visible: false
        }
        AppearanceSettingsPage {
            width: parent.width
            visible: false
        }
        SaveSettingsPage {
            width: parent.width
            visible: false
        }
        SourcesSettingsPage {
            width: parent.width
            visible: false
        }
        BlacklistSettingsPage {
            width: parent.width
            visible: false
        }
        NetworkSettingsPage {
            width: parent.width
            visible: false
        }
        AboutSettingsPage {
            width: parent.width
            visible: false
        }
    }

    Keys.onReleased: {
        if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
            if (stackView.depth > 1) {
                stackView.pop()
            } else {
                root.closed()
            }
            event.accepted = true
        }
    }
}

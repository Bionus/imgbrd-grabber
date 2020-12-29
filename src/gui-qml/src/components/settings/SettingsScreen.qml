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
                    component: "InterfaceSettingsPage.qml"
                }
                ListElement {
                    name: qsTr("Appearance")
                    icon: "/images/icons/palette.png"
                    component: "AppearanceSettingsPage.qml"
                }
                ListElement {
                    name: qsTr("Save")
                    icon: "/images/icons/download.png"
                    component: "SaveSettingsPage.qml"
                }
                ListElement {
                    name: qsTr("Sources")
                    icon: "/images/icons/cloud.png"
                    component: "SourcesSettingsPage.qml"
                }
                ListElement {
                    name: qsTr("Blacklist")
                    icon: "/images/icons/block.png"
                    component: "BlacklistSettingsPage.qml"
                }
                ListElement {
                    name: qsTr("Network")
                    icon: "/images/icons/network.png"
                    component: "NetworkSettingsPage.qml"
                }
                ListElement {
                    name: qsTr("Advanced")
                    icon: "/images/icons/settings.png"
                    component: "AdvancedSettingsPage.qml"
                }
                ListElement {
                    name: qsTr("About")
                    icon: "/images/icons/info.png"
                    component: "AboutSettingsPage.qml"
                }
            }
            delegate: ItemDelegate {
                width: parent.width

                text: model.name
                icon.source: model.icon

                onClicked: stackView.push(settingsPage, { path: model.component })
            }
        }
    }

    Component {
        id: settingsPage

        ScrollView {
            property string path

            width: parent.width
            contentWidth: width

            Loader {
                anchors.fill: parent

                source: "pages/" + path
            }
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

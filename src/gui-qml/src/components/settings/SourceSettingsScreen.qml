import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import Qt.labs.platform 1.1

import "./items"

Page {
    id: root

    signal closed()

    property var site

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.source: "/images/icons/back.png"
                onClicked: root.closed()
            }

            Label {
                text: root.site.name || root.site.url
                elide: Label.ElideRight
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }
    }

    ColumnLayout {
        width: parent.width
        spacing: 0

        SettingTitle {
            Layout.fillWidth: true
            text: qsTr("General")
        }
        TextFieldSetting {
            name: qsTr("Name")
            setting: Setting {
                key: "name"
                def: root.site.url
                obj: root.site.settings
            }
            Layout.fillWidth: true
        }
        CheckBoxSetting {
            name: qsTr("HTTPS")
            subtitle: qsTr("Use a secure connection.")
            setting: Setting {
                key: "ssl"
                def: false
                obj: root.site.settings
            }
            Layout.fillWidth: true
        }

        SettingTitle {
            Layout.fillWidth: true
            text: qsTr("Login")
        }
        Setting {
            id: loginTypeSetting
            key: "login/type"
            def: "url"
            obj: root.site.settings
        }
        ComboSetting {
            name: qsTr("Type")
            options: root.site.authFields.map(f => globals.authTypes[f.type] || f.type)
            values: root.site.authFields.map(f => f.type)
            setting: loginTypeSetting
            Layout.fillWidth: true
        }
        Repeater {
            model: root.site.authFields
            Layout.fillWidth: true

            delegate: Item {
                height: 100
                width: parent.width
                visible: modelData.type === loginTypeSetting.value

                Column {
                    spacing: 0
                    width: parent.width

                    Repeater {
                        model: modelData.fields
                        width: parent.width

                        delegate: TextFieldSetting {
                            name: globals.authFieldLabels[modelData.id] || modelData.id
                            setting: Setting {
                                key: "auth/" + modelData.id
                                def: modelData.def
                                obj: root.site.settings
                            }
                            echoMode: modelData.isPassword ? TextInput.Password : TextInput.Normal
                            width: parent.width
                        }
                    }
                }
            }
        }

        SettingTitle {
            Layout.fillWidth: true
            text: qsTr("API order")
        }
        Setting {
            id: useDefaultApiOrderSetting
            key: "sources/usedefault"
            def: true
            obj: root.site.settings
        }
        CheckBoxSetting {
            name: qsTr("Use default API order")
            setting: useDefaultApiOrderSetting
            Layout.fillWidth: true
        }
        ComboSetting {
            name: qsTr("Source 1")
            options: ["", ...globals.apiTypes]
            values: ["", ...globals.apiTypesKeys]
            setting: Setting {
                key: "sources/source_1"
                def: gSettings.source1.value
                obj: root.site.settings
            }
            Layout.fillWidth: true
            visible: !useDefaultApiOrderSetting.value
        }
        ComboSetting {
            name: qsTr("Source 2")
            options: ["", ...globals.apiTypes]
            values: ["", ...globals.apiTypesKeys]
            setting: Setting {
                key: "sources/source_2"
                def: gSettings.source2.value
                obj: root.site.settings
            }
            Layout.fillWidth: true
            visible: !useDefaultApiOrderSetting.value
        }
        ComboSetting {
            name: qsTr("Source 3")
            options: ["", ...globals.apiTypes]
            values: ["", ...globals.apiTypesKeys]
            setting: Setting {
                key: "sources/source_3"
                def: gSettings.source3.value
                obj: root.site.settings
            }
            Layout.fillWidth: true
            visible: !useDefaultApiOrderSetting.value
        }
        ComboSetting {
            name: qsTr("Source 4")
            options: ["", ...globals.apiTypes]
            values: ["", ...globals.apiTypesKeys]
            setting: Setting {
                key: "sources/source_4"
                def: gSettings.source4.value
                obj: root.site.settings
            }
            Layout.fillWidth: true
            visible: !useDefaultApiOrderSetting.value
        }

        Item {
            Layout.fillHeight: true
        }
    }
}

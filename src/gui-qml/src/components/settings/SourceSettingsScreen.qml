import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import Qt.labs.platform 1.1

import "./items"

Page {
    id: root

    signal closed()

    property string siteUrl
    property var settings

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.source: "/images/icons/back.png"
                onClicked: root.closed()
            }

            Label {
                text: root.siteUrl
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
                def: root.siteUrl
                obj: root.settings
            }
            Layout.fillWidth: true
        }
        CheckBoxSetting {
            name: qsTr("HTTPS")
            subtitle: qsTr("Use a secure connection.")
            setting: Setting {
                key: "ssl"
                def: false
                obj: root.settings
            }
            Layout.fillWidth: true
        }

        SettingTitle {
            Layout.fillWidth: true
            text: qsTr("API order")
        }
        Setting {
            id: useDefaultApiOrderSetting
            key: "sources/usedefault"
            def: true
            obj: root.settings
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
                obj: root.settings
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
                obj: root.settings
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
                obj: root.settings
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
                obj: root.settings
            }
            Layout.fillWidth: true
            visible: !useDefaultApiOrderSetting.value
        }

        Item {
            Layout.fillHeight: true
        }
    }
}

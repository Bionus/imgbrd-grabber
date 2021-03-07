import QtQuick 2.12
import QtQuick.Layouts 1.12

import "../items"

ColumnLayout {
    spacing: 0

    SettingTitle {
        Layout.fillWidth: true
        text: qsTr("API order")
    }
    ComboSetting {
        name: qsTr("Source 1")
        options: globals.apiTypes
        values: globals.apiTypesKeys
        setting: gSettings.source1
        Layout.fillWidth: true
    }
    ComboSetting {
        name: qsTr("Source 2")
        options: globals.apiTypes
        values: globals.apiTypesKeys
        setting: gSettings.source2
        Layout.fillWidth: true
    }
    ComboSetting {
        name: qsTr("Source 3")
        options: globals.apiTypes
        values: globals.apiTypesKeys
        setting: gSettings.source3
        Layout.fillWidth: true
    }
    ComboSetting {
        name: qsTr("Source 4")
        options: globals.apiTypes
        values: globals.apiTypesKeys
        setting: gSettings.source4
        Layout.fillWidth: true
    }

    Item {
        Layout.fillHeight: true
    }
}

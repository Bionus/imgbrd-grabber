import QtQuick 2.12
import QtQuick.Layouts 1.12

import "../items"

ColumnLayout {
    spacing: 0

    SettingTitle {
        Layout.fillWidth: true
        text: qsTr("Appearance")
    }
    RadioSetting {
        name: qsTr("Theme")
        options: globals.materialThemesStr
        values: globals.materialThemes
        setting: gSettings.appearance_materialTheme
        Layout.fillWidth: true
    }
    MaterialColorSetting {
        name: qsTr("Primary color")
        setting: gSettings.appearance_materialPrimary
        Layout.fillWidth: true
    }
    MaterialColorSetting {
        name: qsTr("Accent color")
        setting: gSettings.appearance_materialAccent
        Layout.fillWidth: true
    }

    SettingTitle {
        Layout.fillWidth: true
        text: qsTr("Image")
    }
    ColorPickerSetting {
        name: qsTr("Background color")
        setting: gSettings.imageBackgroundColor
        Layout.fillWidth: true
    }
    SettingItem {
        name: qsTr("Tags")
        Layout.fillWidth: true
        onClicked: openSettingsPage("AppearanceTagsSettingsPage.qml")
    }

    Item {
        Layout.fillHeight: true
    }
}

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
    ComboSetting {
        name: qsTr("Primary color")
        options: globals.materialColorsStr
        values: globals.materialColors
        setting: gSettings.appearance_materialPrimary
        Layout.fillWidth: true
    }
    ComboSetting {
        name: qsTr("Accent color")
        options: globals.materialColorsStr
        values: globals.materialColors
        setting: gSettings.appearance_materialAccent
        Layout.fillWidth: true
    }

    Item {
        Layout.fillHeight: true
    }
}

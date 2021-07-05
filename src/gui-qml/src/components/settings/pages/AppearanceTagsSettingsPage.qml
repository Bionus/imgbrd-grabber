import QtQuick 2.12
import QtQuick.Layouts 1.12

import "../items"

ColumnLayout {
    spacing: 0

    SettingTitle {
        Layout.fillWidth: true
        text: qsTr("Tags")
    }
    ColorPickerSetting {
        name: qsTr("Artists")
        setting: gSettings.coloring_colors_artists
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Circle")
        setting: gSettings.coloring_colors_circles
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Series")
        setting: gSettings.coloring_colors_copyrights
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Characters")
        setting: gSettings.coloring_colors_characters
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Species")
        setting: gSettings.coloring_colors_species
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Metas")
        setting: gSettings.coloring_colors_metas
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Models")
        setting: gSettings.coloring_colors_models
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Generals")
        setting: gSettings.coloring_colors_generals
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Favorites")
        setting: gSettings.coloring_colors_favorites
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Kept for later")
        setting: gSettings.coloring_colors_keptForLater
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Blacklisted")
        setting: gSettings.coloring_colors_blacklisteds
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Ignored")
        setting: gSettings.coloring_colors_ignoreds
        Layout.fillWidth: true
    }

    Item {
        Layout.fillHeight: true
    }
}

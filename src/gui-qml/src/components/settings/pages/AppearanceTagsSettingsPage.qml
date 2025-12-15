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
        preview: true
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Circle")
        setting: gSettings.coloring_colors_circles
        preview: true
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Series")
        setting: gSettings.coloring_colors_copyrights
        preview: true
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Characters")
        setting: gSettings.coloring_colors_characters
        preview: true
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Species")
        setting: gSettings.coloring_colors_species
        preview: true
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Metas")
        setting: gSettings.coloring_colors_metas
        preview: true
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Models")
        setting: gSettings.coloring_colors_models
        preview: true
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Generals")
        setting: gSettings.coloring_colors_generals
        preview: true
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Favorites")
        setting: gSettings.coloring_colors_favorites
        preview: true
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Kept for later")
        setting: gSettings.coloring_colors_keptForLater
        preview: true
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Blacklisted")
        setting: gSettings.coloring_colors_blacklisteds
        preview: true
        Layout.fillWidth: true
    }
    ColorPickerSetting {
        name: qsTr("Ignored")
        setting: gSettings.coloring_colors_ignoreds
        preview: true
        Layout.fillWidth: true
    }

    Item {
        Layout.fillHeight: true
    }
}

import QtQuick 2.12
import QtQuick.Layouts 1.12

import "../items"

ColumnLayout {
    spacing: 0

    SettingTitle {
        Layout.fillWidth: true
        text: qsTr("Interface")
    }
    ComboSetting {
        name: qsTr("Language")
        options: ["English", "French"]
        setting: gSettings.language
        Layout.fillWidth: true

        onChanged: languageLoader.setLanguage(value)
    }

    SettingTitle {
        Layout.fillWidth: true
        text: qsTr("Search results")
    }
    SpinBoxSetting {
        name: qsTr("Columns")
        min: 1
        max: 10
        setting: gSettings.resultsColumnCount
        Layout.fillWidth: true
    }
    RadioSetting {
        name: qsTr("Layout type")
        setting: gSettings.resultsLayoutType
        options: ["Flow", "Grid"]
        values: ["flow", "grid"]
        Layout.fillWidth: true
    }
    RadioSetting {
        name: qsTr("Grid ratio")
        setting: gSettings.resultsHeightToWidthRatio
        options: ["4:3", "1:1", "3:4"]
        values: [0.75, 1, 1.333]
        Layout.fillWidth: true
        visible: gSettings.resultsLayoutType.value === "grid"
    }
    CheckBoxSetting {
        name: qsTr("Spaced grid")
        subtitle: qsTr("Add space between thumbnails.")
        setting: gSettings.resultsAddSpaceBetweenImages
        Layout.fillWidth: true
    }
    CheckBoxSetting {
        name: qsTr("Rounded grid")
        subtitle: qsTr("Slightly round thumbnails.")
        setting: gSettings.resultsRoundImages
        Layout.fillWidth: true
    }

    SettingTitle {
        Layout.fillWidth: true
        text: qsTr("Confirm exit")
    }
    CheckBoxSetting {
        name: qsTr("Confirm exit")
        subtitle: qsTr("Show a confirmation dialog before exiting.")
        setting: gSettings.mobile_confirmExit
        Layout.fillWidth: true
    }
    CheckBoxSetting {
        name: qsTr("Double tap to exit")
        subtitle: qsTr("Tap back button twice to exit.")
        setting: gSettings.mobile_doubleBackExit
        Layout.fillWidth: true
        visible: !gSettings.mobile_confirmExit.value
    }

    Item {
        Layout.fillHeight: true
    }
}

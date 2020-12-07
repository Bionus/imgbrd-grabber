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
    }

    SettingTitle {
        Layout.fillWidth: true
        text: qsTr("Search results")
    }
    SpinBoxSetting {
        name: qsTr("Columns (portrait)")
        min: 1
        max: 10
        setting: gSettings.resultsColumnCountPortrait
        Layout.fillWidth: true
    }
    SpinBoxSetting {
        name: qsTr("Columns (landscape)")
        min: 1
        max: 10
        setting: gSettings.resultsColumnCountLandscape
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
    RadioSetting {
        name: qsTr("Thumbnail fill mode")
        setting: gSettings.resultsThumbnailFillMode
        options: ["Crop", "Fit"]
        values: ["crop", "fit"]
        Layout.fillWidth: true
        visible: gSettings.resultsLayoutType.value === "grid"
    }
    ComboSetting {
        name: qsTr("Spaced grid")
        options: ["None", "Minimal", "Medium"]
        values: ["none", "minimal", "medium"]
        setting: gSettings.resultsSpaceBetweenImages
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
        text: qsTr("Image viewer")
    }
    CheckBoxSetting {
        name: qsTr("Buttons at the bottom")
        subtitle: qsTr("Move the action buttons to the bottom of the screen.")
        setting: gSettings.zoom_buttonsAtBottom
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

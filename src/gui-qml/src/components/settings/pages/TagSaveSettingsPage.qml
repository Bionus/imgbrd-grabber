import QtQuick 2.12
import QtQuick.Layouts 1.12

import ".."
import "../items"

ColumnLayout {
    property string key
    property bool enableShorter: false
    property string defaultEmpty
    property string defaultMultiple
    property string defaultSeparator: "+"

    spacing: 0

    TextFieldSetting {
        name: qsTr("If empty")
        setting: Setting {
            key: "Save/" + key + "_empty"
            def: defaultEmpty
        }
        Layout.fillWidth: true
    }
    TextFieldSetting {
        name: qsTr("Separator")
        setting: Setting {
            key: "Save/" + key + "_sep"
            def: defaultSeparator
        }
        Layout.fillWidth: true
    }
    RadioSetting {
        name: qsTr("Sort")
        options: ["Original", "Name"]
        values: ["original", "name"]
        setting: Setting {
            key: "Save/" + key + "_sort"
            def: "original"
        }
        Layout.fillWidth: true
    }
    SpinBoxSetting {
        name: qsTr("If more than n tags")
        setting: Setting {
            key: "Save/" + key + "_multiple_limit"
            def: 1
        }
        Layout.fillWidth: true
    }

    Setting {
        id: multipleSetting
        key: "Save/" + key + "_multiple"
        def: "keepAll"
    }
    RadioSetting {
        name: qsTr("Action")
        options: ["Keep all tags", "Keep n tags", "Keep n tags, then add", "Replace all tags by", "One file per tag"]
        values: ["keepAll", "keepN", "keepNThenAdd", "replaceAll", "multiple"]
        setting: multipleSetting
        Layout.fillWidth: true
    }
    SpinBoxSetting {
        name: qsTr("Keep n tags")
        visible: multipleSetting.value === "keepN"
        setting: Setting {
            key: "Save/" + key + "_multiple_keepN"
            def: 1
        }
        Layout.fillWidth: true
    }
    SpinBoxSetting {
        name: qsTr("Keep n tags")
        visible: multipleSetting.value === "keepNThenAdd"
        setting: Setting {
            key: "Save/" + key + "_multiple_keepNThenAdd_keep"
            def: 1
        }
        Layout.fillWidth: true
    }
    TextFieldSetting {
        name: qsTr("Then add")
        visible: multipleSetting.value === "keepNThenAdd"
        setting: Setting {
            key: "Save/" + key + "_multiple_keepNThenAdd_add"
            def: " (+ %count%)"
        }
        Layout.fillWidth: true
    }
    TextFieldSetting {
        name: qsTr("Replace all tags by")
        visible: multipleSetting.value === "replaceAll"
        setting: Setting {
            key: "Save/" + key + "_value"
            def: defaultMultiple
        }
        Layout.fillWidth: true
    }

    CheckBoxSetting {
        name: qsTr("Use shortest if possible")
        visible: enableShorter
        setting: Setting {
            key: "Save/" + key + "_useshorter"
            def: true
        }
        Layout.fillWidth: true
    }

    Item {
        Layout.fillHeight: true
    }
}

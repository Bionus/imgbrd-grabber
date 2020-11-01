import QtQuick 2.12
import QtQuick.Layouts 1.12

import "../items"

ColumnLayout {
    spacing: 0

    SettingTitle {
        Layout.fillWidth: true
        text: qsTr("Save")
    }
    FolderSetting {
        name: qsTr("Folder")
        setting: gSettings.save_path
        Layout.fillWidth: true
    }
    TextFieldSetting {
        name: qsTr("Filename")
        setting: gSettings.save_filename
        Layout.fillWidth: true
    }
    TextFieldSetting {
        name: qsTr("Tags separator")
        setting: gSettings.save_separator
        Layout.fillWidth: true
    }
    CheckBoxSetting {
        name: qsTr("Replace spaces by underscores")
        setting: gSettings.save_replaceblanks
        Layout.fillWidth: true
    }
    CheckBoxSetting {
        name: qsTr("Replace JPEG by JPG")
        subtitle: qsTr("If the image's extension is \".jpeg\", it will be replaced by \".jpg\".")
        setting: gSettings.save_noJpeg
        Layout.fillWidth: true
    }

    SettingTitle {
        Layout.fillWidth: true
        text: qsTr("Duplicate management")
    }
    ComboSetting {
        name: qsTr("If a file already exists globally")
        options: [qsTr("Save"), qsTr("Copy"), qsTr("Move"), qsTr("Don't save")]
        values: ["save", "copy", "move", "ignore"]
        setting: gSettings.save_md5Duplicates
        Layout.fillWidth: true
    }
    ComboSetting {
        name: qsTr("If it's in the same directory")
        options: [qsTr("Save"), qsTr("Copy"), qsTr("Move"), qsTr("Don't save")]
        values: ["save", "copy", "move", "ignore"]
        setting: gSettings.save_md5DuplicatesSameDir
        Layout.fillWidth: true
    }
    CheckBoxSetting {
        name: qsTr("Keep deleted files in the MD5 list")
        setting: gSettings.save_keepDeletedMd5
        Layout.fillWidth: true
    }

    Item {
        Layout.fillHeight: true
    }
}

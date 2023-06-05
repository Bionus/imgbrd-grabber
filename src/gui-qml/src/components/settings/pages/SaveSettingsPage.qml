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
    RadioSetting {
        name: qsTr("If a file already exists globally")
        options: [qsTr("Save"), qsTr("Copy"), qsTr("Move"), qsTr("Don't save")]
        values: ["save", "copy", "move", "ignore"]
        setting: gSettings.save_md5Duplicates
        Layout.fillWidth: true
    }
    RadioSetting {
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

    SettingTitle {
        Layout.fillWidth: true
        text: qsTr("Tags")
    }
    Repeater {
        model: ListModel {
            ListElement {
                name: globals.tagTypes["general"]
                key: "general"
                enableShorter: false
                defaultEmpty: ""
                defaultMultiple: ""
                defaultSeparator: " "
            }
            ListElement {
                name: globals.tagTypes["artist"]
                key: "artist"
                enableShorter: false
                defaultEmpty: "anonymous"
                defaultMultiple: "multiple artists"
                defaultSeparator: "+"
            }
            ListElement {
                name: globals.tagTypes["copyright"]
                key: "copyright"
                enableShorter: true
                defaultEmpty: "misc"
                defaultMultiple: "crossover"
                defaultSeparator: "+"
            }
            ListElement {
                name: globals.tagTypes["character"]
                key: "character"
                enableShorter: false
                defaultEmpty: "unknown"
                defaultMultiple: "group"
                defaultSeparator: "+"
            }
            ListElement {
                name: globals.tagTypes["model"]
                key: "model"
                enableShorter: false
                defaultEmpty: "unknown"
                defaultMultiple: "multiple"
                defaultSeparator: "+"
            }
            ListElement {
                name: globals.tagTypes["photo_set"]
                key: "photo_set"
                enableShorter: false
                defaultEmpty: "unknown"
                defaultMultiple: "multiple"
                defaultSeparator: "+"
            }
            ListElement {
                name: globals.tagTypes["species"]
                key: "species"
                enableShorter: false
                defaultEmpty: "unknown"
                defaultMultiple: "multiple"
                defaultSeparator: "+"
            }
            ListElement {
                name: globals.tagTypes["meta"]
                key: "meta"
                enableShorter: false
                defaultEmpty: "none"
                defaultMultiple: "multiple"
                defaultSeparator: "+"
            }
        }
        delegate: SettingItem {
            Layout.fillWidth: true

            name: model.name

            onClicked: settingsStackView.push(tagSaveSettingsPage, {
                key: model.key,
                enableShorter: model.enableShorter,
                defaultEmpty: model.defaultEmpty,
                defaultMultiple: model.defaultMultiple,
                defaultSeparator: model.defaultSeparator
            })
        }
    }

    Component {
        id: tagSaveSettingsPage

        TagSaveSettingsPage {}
    }

    Item {
        Layout.fillHeight: true
    }
}

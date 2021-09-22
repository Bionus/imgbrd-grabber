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
                name: qsTr("Artist")
                key: "artist"
                enableShorter: false
                defaultEmpty: "anonymous"
                defaultMultiple: "multiple artists"
            }
            ListElement {
                name: qsTr("Copyright")
                key: "copyright"
                enableShorter: true
                defaultEmpty: "misc"
                defaultMultiple: "crossover"
            }
            ListElement {
                name: qsTr("Character")
                key: "character"
                enableShorter: false
                defaultEmpty: "unknown"
                defaultMultiple: "group"
            }
            ListElement {
                name: qsTr("Model")
                key: "model"
                enableShorter: false
                defaultEmpty: "unknown"
                defaultMultiple: "multiple"
            }
            ListElement {
                name: qsTr("Photo set")
                key: "photo_set"
                enableShorter: false
                defaultEmpty: "unknown"
                defaultMultiple: "multiple"
            }
            ListElement {
                name: qsTr("Species")
                key: "species"
                enableShorter: false
                defaultEmpty: "unknown"
                defaultMultiple: "multiple"
            }
            ListElement {
                name: qsTr("Meta")
                key: "meta"
                enableShorter: false
                defaultEmpty: "none"
                defaultMultiple: "multiple"
            }
        }
        delegate: SettingItem {
            Layout.fillWidth: true

            name: model.name

            onClicked: settingsStackView.push(tagSaveSettingsPage, {
                key: model.key,
                enableShorter: model.enableShorter,
                defaultEmpty: model.defaultEmpty,
                defaultMultiple: model.defaultMultiple
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

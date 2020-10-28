import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Page {
    id: root

    signal closed()

    property string currentSource
    property var sources

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.source: "/images/icons/back.png"
                onClicked: stackView.depth > 1 ? stackView.pop() : root.closed()
            }

            Label {
                text: qsTr("Settings")
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: mainSettings

        ListView {
            id: mainSettings

            model: ListModel {
                ListElement {
                    name: qsTr("Interface")
                    icon: "/images/icons/interface.png"
                }
                ListElement {
                    name: qsTr("Save")
                    icon: "/images/icons/download.png"
                }
                ListElement {
                    name: qsTr("About")
                    icon: "/images/icons/info.png"
                }
            }
            delegate: ItemDelegate {
                width: parent.width

                text: model.name
                icon.source: model.icon

                onClicked: stackView.push(stackView.children[index + 1])
            }
        }

        ColumnLayout {
            width: parent.width
            spacing: 0
            visible: false

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
            CheckBoxSetting {
                name: qsTr("Hide blacklisted")
                subtitle: qsTr("Hide blacklisted images from the results.")
                setting: gSettings.hideblacklisted
                Layout.fillWidth: true
            }

            Item {
                Layout.fillHeight: true
            }
        }

        ColumnLayout {
            width: parent.width
            spacing: 0
            visible: false

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
                subtitle: qsTr(`If the image's extension is ".jpeg", it will be replaced by ".jpg".`)
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

        ColumnLayout {
            width: parent.width
            spacing: 0
            visible: false

            SettingTitle {
                Layout.fillWidth: true
                text: qsTr("About")
            }
            SettingItem {
                name: qsTr("Version")
                subtitle: NIGHTLY
                    ? "Nightly " + NIGHTLY_COMMIT.substring(0, 8)
                    : "Version " + VERSION
                Layout.fillWidth: true
            }
            LinkSettingItem {
                name: qsTr("See project on Github")
                url: "https://github.com/Bionus/imgbrd-grabber"
                Layout.fillWidth: true
            }
            LinkSettingItem {
                name: qsTr("Report an issue")
                url: "https://github.com/Bionus/imgbrd-grabber/issues/new/choose"
                Layout.fillWidth: true
            }

            SettingTitle {
                Layout.fillWidth: true
                text: qsTr("Author")
            }
            LinkSettingItem {
                name: qsTr("Email")
                subtitle: "bio.nus@hotmail.fr"
                url: "mailto:bio.nus@hotmail.fr"
                Layout.fillWidth: true
            }
            LinkSettingItem {
                name: qsTr("Github")
                subtitle: "@Bionus"
                url: "https://github.com/Bionus"
                Layout.fillWidth: true
            }

            SettingTitle {
                Layout.fillWidth: true
                text: qsTr("Donate")
            }
            LinkSettingItem {
                name: qsTr("Patreon")
                url: "https://www.patreon.com/bionus"
                Layout.fillWidth: true
            }
            LinkSettingItem {
                name: qsTr("Paypal")
                url: "https://www.paypal.me/jvasti"
                Layout.fillWidth: true
            }
            LinkSettingItem {
                name: qsTr("Github")
                url: "https://github.com/sponsors/Bionus"
                Layout.fillWidth: true
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }
}

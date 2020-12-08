import QtQuick 2.12
import QtQuick.Layouts 1.12

import "../items"

ColumnLayout {
    spacing: 0

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
        subtitle: "@bionus"
        url: "https://www.patreon.com/bionus"
        Layout.fillWidth: true
    }
    LinkSettingItem {
        name: qsTr("Paypal")
        subtitle: "@jvasti"
        url: "https://www.paypal.me/jvasti"
        Layout.fillWidth: true
    }
    LinkSettingItem {
        name: qsTr("Github")
        subtitle: "@Bionus"
        url: "https://github.com/sponsors/Bionus"
        Layout.fillWidth: true
    }

    Item {
        Layout.fillHeight: true
    }
}

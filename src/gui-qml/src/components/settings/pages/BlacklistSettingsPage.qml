import QtQuick 2.12
import QtQuick.Layouts 1.12

import "../items"

ColumnLayout {
    spacing: 0

    SettingTitle {
        Layout.fillWidth: true
        text: qsTr("Blacklist")
    }
    TextAreaSetting {
        name: qsTr("Blacklist")
        subtitle: qsTr("One line per blacklist. Multiple tags make an 'AND' condition.")
        setting: gSettings.blacklist
        Layout.fillWidth: true
    }
    CheckBoxSetting {
        name: qsTr("Hide blacklisted")
        subtitle: qsTr("Hide blacklisted images from the results.")
        setting: gSettings.hideblacklisted
        Layout.fillWidth: true
    }
    CheckBoxSetting {
        name: qsTr("Download blacklisted")
        subtitle: qsTr("Download blacklisted images during batch downloads.")
        setting: gSettings.downloadblacklist
        Layout.fillWidth: true
    }

    SettingTitle {
        Layout.fillWidth: true
        text: qsTr("Tagging")
    }
    TextAreaSetting {
        name: qsTr("Removed tags")
        subtitle: qsTr("These won't be taken into account when saving the image.")
        setting: gSettings.removedTags
        Layout.fillWidth: true
    }
    TextAreaSetting {
        name: qsTr("Ignored tags")
        subtitle: qsTr("One per line. Their tag type will be reset to the default.")
        setting: gSettings.ignoredTags
        Layout.fillWidth: true
    }

    Item {
        Layout.fillHeight: true
    }
}

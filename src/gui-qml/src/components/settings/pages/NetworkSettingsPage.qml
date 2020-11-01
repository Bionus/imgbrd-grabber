import QtQuick 2.12
import QtQuick.Layouts 1.12

import "../items"

ColumnLayout {
    spacing: 0

    SettingTitle {
        Layout.fillWidth: true
        text: qsTr("Proxy")
    }
    CheckBoxSetting {
        name: qsTr("Enable proxy")
        setting: gSettings.proxy_use
        Layout.fillWidth: true
    }
    CheckBoxSetting {
        name: qsTr("Use system-wide proxy settings")
        setting: gSettings.proxy_useSystem
        Layout.fillWidth: true
        visible: gSettings.proxy_use.value
    }
    ComboSetting {
        name: qsTr("Type")
        options: ["HTTP", "Socks v5"]
        values: ["http", "socks5"]
        setting: gSettings.proxy_type
        Layout.fillWidth: true
        visible: gSettings.proxy_use.value && !gSettings.proxy_useSystem.value
    }
    TextFieldSetting {
        name: qsTr("Host")
        setting: gSettings.proxy_hostName
        Layout.fillWidth: true
        visible: gSettings.proxy_use.value && !gSettings.proxy_useSystem.value
    }
    TextFieldSetting {
        name: qsTr("Port")
        inputMethodHints: Qt.ImhDigitsOnly
        setting: gSettings.proxy_port
        Layout.fillWidth: true
        visible: gSettings.proxy_use.value && !gSettings.proxy_useSystem.value
    }
    TextFieldSetting {
        name: qsTr("Username")
        setting: gSettings.proxy_user
        Layout.fillWidth: true
        visible: gSettings.proxy_use.value && !gSettings.proxy_useSystem.value
    }
    TextFieldSetting {
        name: qsTr("Password")
        echoMode: TextInput.Password
        setting: gSettings.proxy_password
        Layout.fillWidth: true
        visible: gSettings.proxy_use.value && !gSettings.proxy_useSystem.value
    }

    Item {
        Layout.fillHeight: true
    }
}

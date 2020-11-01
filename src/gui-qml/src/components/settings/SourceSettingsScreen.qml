import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import Qt.labs.platform 1.1

Page {
    id: root

    signal closed()

    property string siteUrl
    property var settings

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.source: "/images/icons/back.png"
                onClicked: root.closed()
            }

            Label {
                text: root.siteUrl
                elide: Label.ElideRight
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }
    }

    ColumnLayout {
        width: parent.width
        spacing: 0

        SettingTitle {
            Layout.fillWidth: true
            text: qsTr("General")
        }

        TextFieldSetting {
            name: qsTr("Name")
            setting: Setting {
                key: "name"
                def: root.siteUrl
                obj: root.settings
            }
            Layout.fillWidth: true
        }

        CheckBoxSetting {
            name: qsTr("HTTPS")
            subtitle: qsTr("Use a secure connection.")
            setting: Setting {
                key: "ssl"
                def: false
                obj: root.settings
            }
            Layout.fillWidth: true
        }
    }
}

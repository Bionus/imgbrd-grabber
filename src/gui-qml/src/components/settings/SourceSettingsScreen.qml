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
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }
    }

    ColumnLayout {
        width: parent.width

        Text {
            Layout.fillWidth: true
            text: qsTr("General")
            font.bold: true
        }

        TextFieldSetting {
            name: qsTr("Name")
            settingKey: "name"
            settingDefault: root.siteUrl
            settingObject: settings
            Layout.fillWidth: true
        }

        CheckBoxSetting {
            name: qsTr("HTTPS")
            subtitle: qsTr("Use a secure connection.")
            settingKey: "ssl"
            settingDefault: false
            settingObject: settings
            Layout.fillWidth: true
        }
    }
}

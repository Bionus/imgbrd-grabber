import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Page {
    property string log

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.source: "/images/icons/menu.png"
                onClicked: drawer.open()
            }

            Label {
                text: qsTr("Log")
                elide: Label.ElideRight
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }
    }

    ScrollView {
        anchors.fill: parent
        clip: true
        padding: 6

        Label {
            anchors.fill: parent
            text: log
            textFormat: Text.RichText
        }
    }
}

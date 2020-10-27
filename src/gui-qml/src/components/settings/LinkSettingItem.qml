import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    id: root

    property alias name: item.name
    property alias subtitle: item.subtitle
    property string url

    implicitHeight: item.implicitHeight

    SettingItem {
        id: item

        name: root.name
        subtitle: setting.value
        anchors.fill: parent

        onClicked: Qt.openUrlExternally(url)
    }
}

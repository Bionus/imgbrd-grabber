import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    id: setting

    signal changed(bool value)

    property string name
    property string subtitle
    property bool value: false

    implicitHeight: item.implicitHeight

    SettingItem {
        id: item

        name: setting.name
        subtitle: setting.subtitle
        anchors.fill: parent

        onClicked: checkBox.toggle()

        CheckBox {
            id: checkBox
            checked: value

            onCheckedChanged: setting.changed(checked)
        }
    }
}

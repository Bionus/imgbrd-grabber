import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    id: root

    property string name
    property string subtitle
    property alias settingKey: setting.settingKey
    property alias settingDefault: setting.settingDefault

    implicitHeight: item.implicitHeight

    Setting {
        id: setting
    }

    SettingItem {
        id: item

        name: root.name
        subtitle: root.subtitle
        anchors.fill: parent

        onClicked: checkBox.toggle()

        CheckBox {
            id: checkBox
            checked: setting.value

            onCheckedChanged: setting.setValue(checked, false)
        }
    }
}

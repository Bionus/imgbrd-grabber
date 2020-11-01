import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.5

Item {
    id: root

    property string name
    property string subtitle
    property Setting setting

    implicitHeight: item.implicitHeight

    SettingItem {
        id: item

        name: root.name
        subtitle: root.subtitle
        anchors.fill: parent

        onClicked: checkBox.toggle()

        CheckBox {
            id: checkBox
            height: 30
            width: 30

            onCheckedChanged: setting.setValue(checked)
            Component.onCompleted: checked = setting.value
        }
    }
}

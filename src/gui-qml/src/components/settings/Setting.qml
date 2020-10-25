import QtQuick 2.12

Item {
    id: root

    signal changed()

    property string settingKey
    property var settingDefault
    property var settingObject: settings

    property string value: root.settingObject.value(settingKey, settingDefault)

    function setValue(val, refresh) {
        root.settingObject.setValue(settingKey, val, settingDefault)
        if (refresh !== false) {
            root.value = val
        }
    }
}

import QtQuick 2.12

Item {
    id: root

    signal changed()

    property string settingKey
    property var settingDefault
    property string value: settings.value(settingKey, settingDefault)

    function setValue(val, refresh) {
        settings.setValue(settingKey, val)
        if (refresh !== false) {
            root.value = val
        }
    }
}

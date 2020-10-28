import QtQuick 2.12

Item {
    id: root

    property string key
    property var def
    property var obj: settings

    property var value: root.obj ? root.obj.value(root.key, root.def) : null

    function setValue(val) {
        root.obj.setValue(root.key, val, root.def)
        root.value = val
    }
}

import QtQuick 2.12

Item {
    id: root

    signal changed(var value)

    property string key
    property var def
    property var obj: settings
    property var parser: null

    property var value: parser !== null ? parser(rawValue) : rawValue
    property var rawValue: root.obj
        ? root.obj.value(root.key, root.def)
        : root.def

    function setValue(val) {
        root.obj.setValue(root.key, val, root.def)
        root.value = val

        root.changed(val)
    }

    Connections {
        target: backend
        function onSettingsChanged() {
            root.rawValue = root.obj.value(root.key, root.def);
            root.value = root.parser !== null ? parser(root.rawValue) : root.rawValue
        }
    }
}

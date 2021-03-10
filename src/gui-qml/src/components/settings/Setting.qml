import QtQuick 2.12

Item {
    id: root

    signal changed(var value)

    property string key
    property var def
    property var obj: settings
    property var parser: null
    property var _parser: parser !== null ? parser : (typeof def === "boolean" ? ((v) => v === true || v === "true") : null)

    property var value: _parser !== null ? _parser(rawValue) : rawValue
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
            root.value = _parser !== null ? _parser(root.rawValue) : root.rawValue
        }
    }
}

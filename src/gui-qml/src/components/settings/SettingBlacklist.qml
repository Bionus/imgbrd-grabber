import QtQuick 2.12

Item {
    id: root

    property string value: backend.getBlacklist()

    function setValue(val) {
        backend.setBlacklist(val)
    }
}

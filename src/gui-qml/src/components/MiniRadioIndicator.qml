// https://github.com/qt/qtdeclarative/blob/v6.6.0/src/quickcontrols/material/impl/RadioIndicator.qml

import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.impl 2.5

Rectangle {
    property Item control
    property int size: 28

    id: indicator
    implicitWidth: size
    implicitHeight: size
    radius: width / 2
    border.width: control.visualFocus ? 2 : 1
    border.color: control.down ? Material.primaryTextColor : Material.secondaryTextColor
    color: "transparent"

    x: control.mirrored ? control.leftPadding : control.width - width - control.rightPadding
    y: control.topPadding + (control.availableHeight - height) / 2

    Rectangle {
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: size - 8
        height: size - 8
        radius: width / 2
        color: Material.primaryTextColor
        visible: indicator.control.checked
    }
}

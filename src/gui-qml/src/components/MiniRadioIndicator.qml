import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.impl 2.5

Rectangle {
    property Item control
    property int size: 28

    implicitWidth: size
    implicitHeight: size

    x: control.mirrored ? control.leftPadding : control.width - width - control.rightPadding
    y: control.topPadding + (control.availableHeight - height) / 2

    radius: width / 2
    color: control.down
        ? (control.visualFocus ? Default.focusPressedColor : Default.indicatorPressedColor)
        : (control.visualFocus ? Default.focusLightColor : Default.backgroundColor)
    border.width: control.visualFocus ? 2 : 1
    border.color: control.visualFocus ? Default.focusColor : (control.down ? Default.indicatorFramePressedColor : Default.indicatorFrameColor)
    opacity: enabled ? 1 : 0.3

    Rectangle {
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: size - 8
        height: size - 8
        radius: width / 2
        color: control.down ? Default.textDarkColor : Default.buttonCheckedColor
        visible: control.checked
    }
}

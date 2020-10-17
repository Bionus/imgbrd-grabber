import QtQuick.Controls 2.5

ScrollView {
    property string log

    Label {
        anchors.fill: parent
        text: log
    }
}

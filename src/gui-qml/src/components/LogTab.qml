import QtQuick 2.12
import QtQuick.Controls 2.5

ScrollView {
    property string log

    clip: true

    Label {
        anchors.fill: parent
        text: log
        textFormat: Text.RichText
    }
}

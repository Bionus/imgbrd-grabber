import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

ColumnLayout {
    id: root

    signal closed()
    property var image

    TabBar {
        id: tabBar
        Layout.fillWidth: true

        TabButton {
            text: "Image"
        }

        TabButton {
            text: "Details"
        }
    }

    SwipeView {
        Layout.fillWidth: true
        Layout.fillHeight: true
        clip: true
        currentIndex: tabBar.currentIndex
        onCurrentIndexChanged: {
            tabBar.currentIndex = currentIndex
        }

        ColumnLayout {
            ZoomableImage {
                id: img
                Layout.fillWidth: true
                Layout.fillHeight: true
                source: image.fileUrl
                clip: true
            }

            ProgressBar {
                value: img.progress
                visible: img.status != Image.Ready
                Layout.fillWidth: true
            }
        }

        ScrollView {
            clip: true

            Text {
                text: image.tags
            }
        }
    }

    DialogButtonBox {
        standardButtons: DialogButtonBox.Close
        Layout.fillWidth: true

        onRejected: root.closed()
    }
}

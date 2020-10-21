import Grabber 1.0
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Page {
    id: root

    signal closed()
    property var image

    ShareUtils {
        id: shareUtils
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.source: "/images/icons/back.png"
                onClicked: root.closed()
            }

            Label {
                text: "Image" // TODO: add more info about the image
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }

            ToolButton {
                icon.source: stackLayout.currentIndex == 0
                    ? "/images/icons/tags.png"
                    : "/images/icons/image.png"
                onClicked: stackLayout.currentIndex = (stackLayout.currentIndex + 1) % 2
            }

            ToolButton {
                icon.source: "/images/icons/share.png"
                onClicked: shareUtils.share("Image", image.fileUrl)
            }

            ToolButton {
                icon.source: "/images/icons/download.png"
                onClicked: backend.downloadImage(image.image)
            }

            ToolButton {
                visible: image.sampleUrl !== image.fileUrl
                icon.source: String(img.source) === image.sampleUrl
                    ? "/images/icons/hd.png"
                    : "/images/icons/ld.png"
                onClicked: img.source = String(img.source) === image.sampleUrl
                    ? image.fileUrl
                    : image.sampleUrl
            }
        }
    }

    StackLayout {
        id: stackLayout
        anchors.fill: parent
        clip: true
        currentIndex: 0

        ColumnLayout {
            ZoomableImage {
                id: img
                Layout.fillWidth: true
                Layout.fillHeight: true
                source: image.sampleUrl
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

            ListView {
                model: image.tags

                delegate: Text {
                    text: modelData
                    textFormat: Text.RichText

                    onLinkActivated: {
                        root.closed()
                        searchTab.load(link)
                    }
                }
            }
        }
    }
}

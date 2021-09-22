import Grabber 1.0
import QtMultimedia 5.12
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

Page {
    id: root

    signal closed()

    property var images
    property int index
    property var image: images[swipeView.currentIndex]

    property bool hasSample: image.sampleUrl !== image.fileUrl && image.sampleUrl !== image.previewUrl
    property bool showHd: !gSettings.zoom_viewSamples.value
    property bool showTags: false

    Component {
        id: actionButtons

        RowLayout {
            ToolButton {
                icon.source: showTags ? "/images/icons/image.png" : "/images/icons/tags.png"
                onClicked: showTags = !showTags
            }

            ToolButton {
                icon.source: "/images/icons/share.png"
                onClicked: backend.shareImage(image.image)
            }

            ImageLoader {
                id: downloader
                image: root.image.image
                automatic: false
	            filename: gSettings.save_filename.value
	            path: gSettings.save_path.value
            }
            ToolButton {
                icon.source: downloader.status == ImageLoader.Ready
                    ? "/images/icons/delete.png"
                    : (downloader.status == ImageLoader.Loading
                        ? "/images/icons/loading.png"
                        : (downloader.status == ImageLoader.Error
                            ? "/images/icons/warning.png"
                            : "/images/icons/download.png"))
                onClicked: downloader.status == ImageLoader.Ready ? downloader.remove() : downloader.load()
            }

            ToolButton {
                visible: hasSample
                icon.source: showHd ? "/images/icons/ld.png" : "/images/icons/hd.png"
                onClicked: showHd = !showHd
            }
        }
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.source: "/images/icons/back.png"
                onClicked: root.closed()
            }

            Label {
                text: qsTr("Image") // TODO: add more info about the image
                elide: Label.ElideRight
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }

            Loader {
                active: !gSettings.zoom_buttonsAtBottom.value
                sourceComponent: actionButtons
            }
        }
    }

    footer: Loader {
        active: gSettings.zoom_buttonsAtBottom.value
        sourceComponent: Component {
            RowLayout {
                Label {
                    Layout.fillWidth: true
                }

                Loader {
                    sourceComponent: actionButtons
                }
            }
        }
    }

    SwipeView {
        id: swipeView
        currentIndex: root.index
        anchors.fill: parent
        clip: true

        onCurrentIndexChanged: {
            showHd = !gSettings.zoom_viewSamples.value
            showTags = false
        }

        Repeater {
            model: root.images

            Loader {
                active: SwipeView.isPreviousItem || SwipeView.isCurrentItem || SwipeView.isNextItem

                sourceComponent: StackLayout {
                    id: stackLayout
                    clip: true
                    currentIndex: showTags && index == swipeView.currentIndex ? 1 : 0

                    Rectangle {
                        color: gSettings.imageBackgroundColor.value || "transparent"

                        Loader {
                            active: !modelData.isVideo
                            anchors.fill: parent

                            sourceComponent: ColumnLayout {
                                ImageLoader {
                                    id: loader
                                    image: modelData.image
                                    size: (showHd || !hasSample ? ImageLoader.Full : ImageLoader.Sample)
                                }

                                ZoomableImage {
                                    id: img
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    source: loader.source
                                    animated: modelData.isAnimated
                                    clip: true
                                }

                                ProgressBar {
                                    value: loader.progress
                                    visible: loader.status != ImageLoader.Ready
                                    Layout.fillWidth: true
                                }
                            }
                        }

                        Loader {
                            active: modelData.isVideo
                            anchors.fill: parent

                            sourceComponent: VideoPlayer {
                                fillMode: VideoOutput.PreserveAspectFit
                                source: showHd || !hasSample ? modelData.fileUrl : modelData.sampleUrl
                                clip: true
                                autoPlay: index == swipeView.currentIndex
                            }
                        }
                    }

                    ScrollView {
                        clip: true
                        padding: 8

                        Label {
                            anchors.fill: parent
                            text: (Material.theme == Material.Dark ? modelData.tagsDark : modelData.tags).join("<br/>")
                            textFormat: Text.RichText

                            onLinkActivated: {
                                root.closed()
                                searchTab.load(decodeURIComponent(link))
                            }
                        }
                    }

                    Component.onCompleted: modelData.loadTags()
                }
            }
        }
    }

    Keys.onReleased: {
        if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
            if (root.showTags) {
                root.showTags = false
            } else {
                root.closed()
            }
            event.accepted = true
        }
    }
}

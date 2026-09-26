import Grabber 1.0
import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import "../vendor"

ScrollView {
    id: root

    signal openImage(int index)
    signal refresh()
    signal appendNext()

    property var results
    property double thumbnailHeightToWidthRatio: 0
    property int thumbnailSpacing: 0
    property int thumbnailRadius: 0
    property bool thumbnailPadding: false
    property var thumbnailFillMode: Image.PreserveAspectFit

    contentHeight: resultsLayout.contentHeight
    clip: true
    padding: thumbnailPadding
        ? root.thumbnailSpacing / 2
        : -root.thumbnailSpacing / 2

    onThumbnailHeightToWidthRatioChanged: resultsRefresher.restart()
    onThumbnailSpacingChanged: resultsRefresher.restart()
    onThumbnailPaddingChanged: resultsRefresher.restart()
    onThumbnailFillModeChanged: resultsRefresher.restart()

    Flickable {
        // Pull to refresh
        property bool atBeginningStart: false
        onFlickStarted: {
            atBeginningStart = atYBeginning
        }
        onFlickEnded: {
            if (atYBeginning && atBeginningStart) {
                refresh()
            }
        }

        // Infinite scroll
        onAtYEndChanged: {
            if (atYEnd) {
                appendNext()
            }
        }

        ColumnFlow {
            id: resultsLayout

            anchors.fill: parent
            columns: window.width > window.height
                ? gSettings.resultsColumnCountLandscape.value
                : gSettings.resultsColumnCountPortrait.value
            model: results

            onColumnsChanged: resultsRefresher.restart()

            delegate: Item {
                readonly property real safeRatio: img.status === Image.Ready
                    && img.implicitWidth > 0
                    && img.implicitHeight > 0
                        ? img.implicitHeight / img.implicitWidth
                        : 1

                height: img.height + root.thumbnailSpacing

                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width - root.thumbnailSpacing
                    height: img.height
                    radius: root.thumbnailRadius
                    color: "transparent"
                    border.color: Material.hintTextColor
                    border.width: 1
                    visible: img.status === Image.Error

                    Text {
                        anchors.centerIn: parent
                        text: "!"
                        color: Material.secondaryTextColor
                        font.pixelSize: Math.max(18, parent.width * 0.18)
                        font.bold: true
                    }
                }

                Image {
                    id: img
                    source: "image://async/" + modelData.siteUrl + "¤" + modelData.smartPreviewUrl(width, height) + "¤" + modelData.previewRect
                    fillMode: root.thumbnailFillMode
                    anchors.centerIn: parent
                    width: parent.width - root.thumbnailSpacing
                    height: root.thumbnailHeightToWidthRatio < 0.1
                        ? img.width * parent.safeRatio
                        : img.width * root.thumbnailHeightToWidthRatio
                    visible: status !== Image.Error

                    onHeightChanged: resultsRefresher.restart()

                    layer.enabled: root.thumbnailRadius > 0
                    layer.effect: OpacityMask {
                        maskSource: Rectangle {
                            anchors.centerIn: parent
                            width: img.width
                            height: img.height
                            radius: root.thumbnailRadius
                        }
                    }

                    Image {
                        property int overlaySize: Math.min(img.width / 3, previewVideoOverlay.implicitWidth)

                        id: previewVideoOverlay
                        visible: gSettings.previewVideoIndicator.value && modelData.isVideo
                        source: "/images/thumbnail-video-overlay.png"
                        width: overlaySize
                        height: overlaySize
                        anchors.centerIn: parent
                    }

                    InnerBorder {
                        visible: modelData.color.a > 0
                        color: modelData.color
                        size: 3
                    }

                    Badge {
                        visible: !!modelData.badge
                        text: modelData.badge
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: modelData.isGallery
                        ? mainStackView.push(galleryScreen, { gallery: modelData })
                        : mainStackView.push(imageScreen, { index: index })
                }
            }
        }

        Timer {
            id: resultsRefresher
            interval: 100
            running: false
            repeat: false

            onTriggered: resultsLayout.reEvalColumns()
        }
    }
}

import Grabber 1.0
import QtQml 2.12
import QtQuick 2.12
import Qt5Compat.GraphicalEffects
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
                height: img.height + root.thumbnailSpacing

                Image {
                    id: img
                    source: "image://async/" + modelData.siteUrl + "¤" + modelData.previewUrl + "¤" + modelData.previewRect
                    fillMode: root.thumbnailFillMode
                    anchors.centerIn: parent
                    width: parent.width - root.thumbnailSpacing
                    height: root.thumbnailHeightToWidthRatio < 0.1
                        ? img.width * (img.implicitHeight / img.implicitWidth)
                        : img.width * root.thumbnailHeightToWidthRatio

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

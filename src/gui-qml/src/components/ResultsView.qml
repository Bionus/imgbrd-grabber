import QtQml 2.12
import QtQuick 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import "../vendor"

ScrollView {
    id: root

    signal openImage(int index)

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
                source: modelData.previewUrl
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
            }

            MouseArea {
                anchors.fill: parent
                onClicked: mainStackView.push(imageScreen, { index: index })
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

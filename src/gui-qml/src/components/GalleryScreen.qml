import Grabber 1.0
import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import "../vendor"

Page {
    id: root

    signal back()

    property int page: 1
    property string site
    property var gallery
    property bool infiniteScroll: gSettings.resultsInfiniteScroll.value
    property var results
    property bool appendResults: false

    GallerySearchLoader {
        id: galleryLoader

        site: root.site
        gallery: root.gallery.image
        page: root.page
        perPage: 20
        profile: backend.profile

        onGalleryChanged: galleryLoader.load()
        onResultsChanged: {
            if (appendResults) {
                appendResults = false
                root.results = root.results.concat(galleryLoader.results)
            } else {
                root.results = galleryLoader.results
            }
        }
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.source: "/images/icons/back.png"
                onClicked: root.back()
            }

            Label {
                text: root.gallery.name
                elide: Label.ElideRight
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }
    }

    Component {
        id: imageScreen

        ImageScreen {
            images: root.results
            index: 0

            onClosed: mainStackView.pop()
        }
    }

    ColumnLayout {
        spacing: 0
        anchors.fill: parent

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            ResultsView {
                results: root.results
                thumbnailHeightToWidthRatio: gSettings.resultsLayoutType.value === "flow" ? 0 : gSettings.resultsHeightToWidthRatio.value
                thumbnailSpacing: gSettings.resultsSpaceBetweenImages.value === "none" ? 0 : (gSettings.resultsSpaceBetweenImages.value === "minimal" ? 2 : 8)
                thumbnailPadding: gSettings.resultsSpaceBetweenImages.value === "medium"
                thumbnailRadius: gSettings.resultsRoundImages.value ? 8 : 0
                thumbnailFillMode: gSettings.resultsLayoutType.value === "grid" && gSettings.resultsThumbnailFillMode.value === "crop" ? Image.PreserveAspectCrop : Image.PreserveAspectFit
                anchors.fill: parent

                onOpenImage: mainStackView.push(imageScreen, { index: index })
                onRefresh: galleryLoader.load()
                onAppendNext: {
                    if (infiniteScroll && galleryLoader.hasNext) {
                        root.appendResults = true
                        root.page++
                        galleryLoader.load()
                    }
                }
            }

            Loading {
                visible: galleryLoader.status == TagSearchLoader.Loading
                anchors.fill: parent
            }
        }

        RowLayout {
            spacing: 0
            Layout.fillWidth: true
            Layout.fillHeight: false

            Button {
                id: prevButton
                background.anchors.fill: prevButton
                width: 40
                icon.source: "/images/icons/previous.png"
                visible: !infiniteScroll
                enabled: query !== "" && page > 1
                Layout.fillHeight: true
                Material.elevation: 0

                onClicked: {
                    page--
                    galleryLoader.load()
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                id: nextButton
                background.anchors.fill: nextButton
                width: 40
                icon.source: "/images/icons/next.png"
                visible: !infiniteScroll
                enabled: query !== ""
                Layout.fillHeight: true
                Material.elevation: 0

                onClicked: {
                    page++
                    galleryLoader.load()
                }
            }
        }
    }
}

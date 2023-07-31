import Grabber 1.0
import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

import "../vendor"

Page {
    id: searchTab

    signal openSources()

    property int page: 1
    property string site
    property bool infiniteScroll: gSettings.resultsInfiniteScroll.value
    property var results
    property bool queryChanged: false
    property bool appendResults: false

    TagSearchLoader {
        id: pageLoader

        site: searchTab.site
        query: textFieldSearch.text
        page: searchTab.page
        perPage: 20
        postFilter: textFieldPostFiltering.text
        profile: backend.profile

        onQueryChanged: searchTab.queryChanged = true
        onResultsChanged: {
            if (appendResults) {
                appendResults = false
                searchTab.results = searchTab.results.concat(pageLoader.results)
            } else {
                searchTab.results = pageLoader.results
            }
        }
    }

    function load(tag) {
        if (tag) {
            textFieldSearch.text = tag.trim()
            queryChanged = true
        }
        if (queryChanged) {
            page = 1
            queryChanged = false
        }
        pageLoader.load()
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.source: "/images/icons/menu.png"
                onClicked: drawer.open()
            }

            SearchField {
                id: textFieldSearch

                text: ""
                placeholderText: qsTr("Search...")
                Layout.fillWidth: true
                Layout.fillHeight: true

                onEnterPressed: searchTab.load()
            }

            ToolButton {
                icon.source: "/images/icons/search.png"
                onClicked: searchTab.load()
            }
        }
    }

    Component {
        id: imageScreen

        ImageScreen {
            images: searchTab.results
            index: 0

            onClosed: mainStackView.pop()
        }
    }

    ColumnLayout {
        spacing: 0
        anchors.fill: parent

        ColumnLayout {
            spacing: 0
            Layout.fillWidth: true
            visible: textFieldSearch.isOpen

            Item {
                Layout.fillWidth: true
                height: 40

                Rectangle {
                    anchors.fill: parent
                    anchors.topMargin: -1
                    anchors.leftMargin: -1
                    anchors.rightMargin: -1
                    anchors.bottomMargin: 0
                    border.width: 1
                    border.color: Qt.rgba(0, 0, 0, 0.4)
                    color: Qt.rgba(1, 1, 1, 0.7)
                    clip: true

                    SearchEdit {
                        id: textFieldPostFiltering
                        placeholderText: "Post-filters"
                        anchors.fill: parent
                        anchors.margins: 8

                        onEnterPressed: searchTab.load()
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                height: 40

                ToolButton {
                    property var isFavorited: Array.prototype.indexOf.call(backend.favorites, pageLoader.query) >= 0
                    icon.source: "/images/icons/" + (isFavorited ? "favorites_filled" : "favorites") + ".png"
                    icon.color: isFavorited ? "pink" : Material.foreground
                    enabled: pageLoader.query !== ""
                    onClicked: isFavorited
                        ? backend.removeFavorite(pageLoader.query)
                        : backend.addFavorite(pageLoader.query, searchTab.site)
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            ResultsView {
                results: searchTab.results
                thumbnailHeightToWidthRatio: gSettings.resultsLayoutType.value === "flow" ? 0 : gSettings.resultsHeightToWidthRatio.value
                thumbnailSpacing: gSettings.resultsSpaceBetweenImages.value === "none" ? 0 : (gSettings.resultsSpaceBetweenImages.value === "minimal" ? 2 : 8)
                thumbnailPadding: gSettings.resultsSpaceBetweenImages.value === "medium"
                thumbnailRadius: gSettings.resultsRoundImages.value ? 8 : 0
                thumbnailFillMode: gSettings.resultsLayoutType.value === "grid" && gSettings.resultsThumbnailFillMode.value === "crop" ? Image.PreserveAspectCrop : Image.PreserveAspectFit
                anchors.fill: parent

                onOpenImage: mainStackView.push(imageScreen, { index: index })
                onRefresh: load()
                onAppendNext: {
                    if (infiniteScroll && pageLoader.hasNext) {
                        searchTab.appendResults = true
                        searchTab.page++
                        searchTab.load()
                    }
                }
            }

            Loading {
                visible: pageLoader.status == TagSearchLoader.Loading
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
                enabled: pageLoader.hasPrev
                Layout.fillHeight: true
                Material.elevation: 0

                onClicked: {
                    page--
                    searchTab.load()
                }
            }

            Button {
                id: sourcesButton
                background.anchors.fill: sourcesButton
                text: qsTr("Sources")
                Layout.fillWidth: true
                Layout.fillHeight: true
                Material.elevation: 0

                onClicked: searchTab.openSources()
            }

            Button {
                id: nextButton
                background.anchors.fill: nextButton
                width: 40
                icon.source: "/images/icons/next.png"
                visible: !infiniteScroll
                enabled: pageLoader.hasNext
                Layout.fillHeight: true
                Material.elevation: 0

                onClicked: {
                    page++
                    searchTab.load()
                }
            }
        }
    }
}

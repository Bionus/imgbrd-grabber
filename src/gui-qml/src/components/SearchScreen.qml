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
    property var site
    property bool infiniteScroll: gSettings.resultsInfiniteScroll.value
    property var results
    property bool queryChanged: false
    property bool appendResults: false

    TagSearchLoader {
        id: pageLoader

        site: searchTab.site.url
        query: (textFieldSearch.text + " " + gSettings.globalAddedTags.value).trim()
        page: searchTab.page
        perPage: 20
        endpoint: site.endpoints[comboEndpoint.currentIndex].id
        postFilter: (textFieldPostFiltering.text + " " + gSettings.globalPostFilters.value).trim()
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
        if (tag !== undefined) {
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

            ComboBox {
                id: comboEndpoint
                visible: gSettings.v8.value
                model: site.endpoints.map(endpoint => endpoint.name)

                Layout.fillWidth: true
                implicitHeight: 40
                topInset: 0
                bottomInset: 0
                background: Rectangle {}
            }

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
                        : backend.addFavorite(pageLoader.query, searchTab.site.url)
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
                visible: pageLoader.status === TagSearchLoader.Loading
                anchors.fill: parent
            }
        }

        RowLayout {
            spacing: 0
            Layout.fillWidth: true
            Layout.fillHeight: false
            //Layout.preferredHeight: 40

            Button {
                id: prevButton
                background.anchors.fill: prevButton
                width: 40
                icon.source: "/images/icons/previous.png"
                visible: !infiniteScroll
                enabled: pageLoader.hasPrev
                Layout.fillHeight: true
                Material.elevation: 0
                Material.roundedScale: Material.NotRounded

                onClicked: {
                    page--
                    searchTab.load()
                }
            }

            Label {
                text: pageLoader.status === TagSearchLoader.Ready
                    ? (results.length > 0
                        ? qsTr("Page %1 of %2\n(%3 of %4)").arg(page).arg(pageLoader.pageCount).arg(results.length).arg(pageLoader.imageCount)
                        : qsTr("No result"))
                    : ''
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                rightPadding: sourcesButton.width
                Layout.fillWidth: true
                Layout.fillHeight: true

                background: Rectangle {
                    color: nextButton.background.color
                }
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
                Material.roundedScale: Material.NotRounded

                onClicked: {
                    page++
                    searchTab.load()
                }
            }
        }
    }
}

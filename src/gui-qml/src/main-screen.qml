import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

import "components"

ApplicationWindow {
    id: root

    visible: true
    width: 300
    height: 500
    title: "Grabber"

    property string site: "danbooru.donmai.us"

    TabBar {
        id: tabBar
        width: parent.width

        TabButton {
            text: "Search"
        }

        TabButton {
            text: "Log"
        }
    }

    StackLayout {
        anchors.topMargin: tabBar.height
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        SearchTab {
            site: root.site
            query: backend.query
            thumbnails: backend.thumbnails

            onOpenSources: sourcesScreen.visible = true
            onSearch: backend.search(site, query, page)
        }

        LogTab {
            log: backend.log
        }
    }

    SourcesScreen {
        id: sourcesScreen
        visible: false
        sources: backend.sites
        anchors.fill: parent

        onAccepted: { site = source; sourcesScreen.visible = false }
        onRejected: sourcesScreen.visible = false
    }
}

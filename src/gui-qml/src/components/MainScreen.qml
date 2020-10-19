import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Item {
    id: root

    signal openSources()
    property string site

    MainDrawer {}

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

    SwipeView {
        anchors.topMargin: tabBar.height
        anchors.fill: parent
        clip: true
        currentIndex: tabBar.currentIndex
        onCurrentIndexChanged: {
            tabBar.currentIndex = currentIndex
        }

        SearchTab {
            site: root.site
            query: backend.query
            results: backend.results

            onOpenSources: root.openSources()
            onSearch: backend.search(site, query, page)
        }

        LogTab {
            log: backend.log
        }
    }
}

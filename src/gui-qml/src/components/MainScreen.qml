import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Item {
    id: root

    signal openSources()
    property string site

    anchors.fill: parent

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

            onOpenSources: root.openSources()
            onSearch: backend.search(site, query, page)
        }

        LogTab {
            log: backend.log
        }
    }
}

import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

import "components"
import "components/settings"

ApplicationWindow {
    id: window

    visible: true
    width: 300
    height: 500
    title: "Grabber"

    property string site: "danbooru.donmai.us"
    property string currentPage: "search"

    MainDrawer {
        id: drawer

        headerTitle: "Grabber"
        headerSubtitle: NIGHTLY
            ? "Nightly " + NIGHTLY_COMMIT.substring(0, 8)
            : "Version " + VERSION
        headerIconSource: "/images/logo.png"

        onChangePage: currentPage = page
        onOpenSettings: mainStackView.push(settingsScreen)
    }

    StackView {
        id: mainStackView
        anchors.fill: parent
        initialItem: mainScreen

        Item {
            id: mainScreen
            anchors.fill: parent

            SearchScreen {
                visible: currentPage == "search"
                anchors.fill: parent
                site: window.site
                query: backend.query
                results: backend.results

                onOpenSources: mainStackView.push(sourcesScreen)
            }

            LogScreen {
                id: logScreen
                visible: currentPage == "log"
                anchors.fill: parent
                log: backend.log
            }
        }

        SourcesScreen {
            id: sourcesScreen
            visible: false
            sources: backend.sites
            currentSource: site

            onAccepted: { site = source; mainStackView.pop() }
            onRejected: mainStackView.pop()
            onAddSource: mainStackView.push(addSourceScreen)
        }

        AddSourceScreen {
            id: addSourceScreen
            visible: false

            onAccepted: { /* backend.refreshSources(); */ mainStackView.pop() }
            onRejected: mainStackView.pop()
        }

        SettingsScreen {
            id: settingsScreen
            visible: false

            onClosed: mainStackView.pop()
        }
    }
}

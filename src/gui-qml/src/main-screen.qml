import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

import "components"

ApplicationWindow {
    id: window

    visible: true
    width: 300
    height: 500
    title: "Grabber"

    property string site: "danbooru.donmai.us"

    StackView {
        id: mainStackView
        anchors.fill: parent
        initialItem: mainScreen

        MainScreen {
            id: mainScreen
            site: window.site

            onOpenSources: mainStackView.push(sourcesScreen)
        }

        SourcesScreen {
            id: sourcesScreen
            visible: false
            sources: backend.sites
            currentSource: site

            onAccepted: { site = source; mainStackView.pop() }
            onRejected: mainStackView.pop()
        }
    }
}

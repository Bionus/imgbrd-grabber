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

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: mainScreen

        MainScreen {
            id: mainScreen
            site: root.site

            onOpenSources: stackView.push(sourcesScreen)
        }

        SourcesScreen {
            id: sourcesScreen
            visible: false
            sources: backend.sites

            onAccepted: { site = source; stackView.pop() }
            onRejected: stackView.pop()
        }
    }
}

import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

import "components"
import "components/settings"

ApplicationWindow {
    id: window

    visible: true
    width: 400
    height: 700
    title: "Grabber"

    property string site: "danbooru.donmai.us"
    property string currentPage: "search"

    Settings {
        id: gSettings
    }

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
        focus: true

        Item {
            id: mainScreen

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

            Dialog {
                id: confirmExitDialog

                title: qsTr("Do you want to exit?")
                anchors.centerIn: Overlay.overlay
                modal: true
                standardButtons: Dialog.Yes | Dialog.Cancel

                onAccepted: {
                    if (dontAskAgain.checked) {
                        gSettings.mobile_confirmExit.setValue(false)
                    }
                    Qt.quit()
                }

                CheckBox {
                    id: dontAskAgain
                    text: qsTr("Don't ask again")
                }
            }

            property double backPressed: 0
            Keys.onReleased: {
                if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
                    var now = new Date().getTime()
                    if (mainStackView.depth > 1) {
                        mainStackView.pop()
                    } else if (gSettings.mobile_confirmExit.value) {
                        confirmExitDialog.open()
                    } else if (gSettings.mobile_doubleBackExit.value && now - backPressed > 200) {
                        backPressed = now
                    } else {
                        Qt.quit()
                    }
                    event.accepted = true
                }
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
            onEditSource: mainStackView.push(editSourceScreen, { siteUrl: source, settings: settingsObject })
        }

        AddSourceScreen {
            id: addSourceScreen
            visible: false

            onAccepted: { /* backend.refreshSources(); */ mainStackView.pop() }
            onRejected: mainStackView.pop()
        }

        Component {
            id: editSourceScreen

            SourceSettingsScreen {
                onClosed: mainStackView.pop()
            }
        }

        SettingsScreen {
            id: settingsScreen
            visible: false

            onClosed: mainStackView.pop()
        }
    }
}

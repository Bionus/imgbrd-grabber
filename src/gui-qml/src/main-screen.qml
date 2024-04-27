import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import StatusBar 0.1 as SB

import "components"
import "components/settings"

ApplicationWindow {
    id: window

    visible: true
    width: 400
    height: 700
    title: "Grabber"

    property string currentPage: "search"
    property var activeSite: backend.sites.filter(site => site.url === gSettings.activeSource.value)[0]

    Material.theme: gSettings.appearance_materialTheme.value
    Material.primary: gSettings.appearance_materialPrimary.value
    Material.accent: gSettings.appearance_materialAccent.value

    SB.StatusBar {
        theme: gSettings.appearance_materialTheme.value
        color: Material.color(gSettings.appearance_materialPrimary.value, Material.Shade700)
        navigationColor: window.color
    }

    Settings {
        id: gSettings
    }

    Item {
        id: globals

        property var materialThemes: [Material.Light, Material.Dark, Material.System]
        property var materialThemesStr: ["Light", "Dark", "System"]

        property var materialColors: [Material.Red, Material.Pink, Material.Purple, Material.DeepPurple, Material.Indigo, Material.Blue, Material.LightBlue, Material.Cyan, Material.Teal, Material.Green, Material.LightGreen, Material.Lime, Material.Yellow, Material.Amber, Material.Orange, Material.DeepOrange, Material.Brown, Material.Grey, Material.BlueGrey]
        property var materialColorsStr: ["Red", "Pink", "Purple", "Deep purple", "Indigo", "Blue", "Light blue", "Cyan", "Teal", "Green", "Light green", "Lime", "Yellow", "Amber", "Orange", "Deep orange", "Brown", "Grey", "Blue grey"]

        property var apiTypes: ["XML", "JSON", "Regex", "RSS"]
        property var apiTypesKeys: ["xml", "json", "regex", "rss"]

        property var tagTypes: {
            "general": qsTr("General"),
            "artist": qsTr("Artist"),
            "copyright": qsTr("Copyright"),
            "character": qsTr("Character"),
            "model": qsTr("Model"),
            "photo_set": qsTr("Photo set"),
            "species": qsTr("Species"),
            "meta": qsTr("Meta")
        }
        property var authTypes: {
            "url": qsTr("Through URL"),
            "http_basic": qsTr("HTTP Basic"),
            "get": qsTr("GET"),
            "post": qsTr("POST"),
            "oauth1": qsTr("OAuth 1"),
            "oauth2": qsTr("OAuth 2")
        }
        property var authFieldLabels: {
            "pseudo": qsTr("Username"),
            "userId": qsTr("User ID"),
            "password": qsTr("Password"),
            "salt": qsTr("Salt"),
            "apiKey": qsTr("API key")
        }
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

    Dialog {
        title: qsTr("Update available")
        anchors.centerIn: Overlay.overlay
        modal: true
        width: 360
        visible: updateChecker.updateAvailable
        standardButtons: Dialog.Cancel | Dialog.Ok

        onAccepted: Qt.openUrlExternally(updateChecker.latestUrl)

        Text {
            anchors.fill: parent
            text: updateChecker.changelog
            wrapMode: Text.WordWrap
            textFormat: TextEdit.MarkdownText
            onLinkActivated: Qt.openUrlExternally(link)
        }
    }

    StackView {
        id: mainStackView
        anchors.fill: parent
        initialItem: mainScreen
        focus: true

        Item {
            id: mainScreen

            SearchScreen {
                id: searchScreen
                visible: currentPage === "search"
                anchors.fill: parent
                site: activeSite

                onOpenSources: mainStackView.push(sourcesScreen)
            }

            FavoritesScreen {
                id: favoritesScreen
                visible: currentPage === "favorites"
                anchors.fill: parent
                favorites: backend.favorites

                onOpenFavorite: {
                    searchScreen.load(favorite)
                    currentPage = "search"
                }
            }

            SourcesScreen {
                visible: currentPage === "sources"
                anchors.fill: parent
                sources: backend.sites
                activeSource: gSettings.activeSource.value

                onActiveSourceChanged: { gSettings.activeSource.setValue(activeSource); }
                onBack: mainStackView.pop()
                onAddSource: mainStackView.push(addSourceScreen)
                onEditSource: mainStackView.push(editSourceScreen, { site: activeSite })
            }

            LogScreen {
                id: logScreen
                visible: currentPage === "log"
                anchors.fill: parent
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
        }

        Component {
            id: galleryScreen

            GalleryScreen {
                site: activeSite
                onBack: mainStackView.pop()
            }
        }

        SourcesScreen {
            id: sourcesScreen
            visible: false
            sources: backend.sites
            activeSource: gSettings.activeSource.value

            onActiveSourceChanged: { gSettings.activeSource.setValue(activeSource); }
            onBack: mainStackView.pop()
            onAddSource: mainStackView.push(addSourceScreen)
            onEditSource: mainStackView.push(editSourceScreen, { site: activeSite })
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

        property double backPressed: 0
        Keys.onReleased: event => {
            if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
                const now = new Date().getTime()
                if (mainStackView.depth > 1) {
                    mainStackView.pop()
                } else if (currentPage !== "search") {
                    currentPage = "search";
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
}

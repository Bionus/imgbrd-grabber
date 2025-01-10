import Grabber 1.0
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

Item {
    id: tagView

    property var image

    Menu {
        id: tagContextMenu
        title: tagContextMenu.tag

        property string tag

        MenuItem {
            property bool isFavorited: Array.prototype.indexOf.call(backend.favorites, tagContextMenu.tag) >= 0

            text: isFavorited ? qsTr("Remove from favorites") : qsTr("Add to favorites")
            icon.source: "/images/icons/" + (isFavorited ? "favorites_filled" : "favorites") + ".png"

            onTriggered: {
                if (isFavorited) {
                    backend.removeFavorite(tagContextMenu.tag)
                } else {
                    backend.addFavorite(tagContextMenu.tag, searchTab.site.url)
                }
            }
        }

        MenuItem {
            text: qsTr("Copy tag")
            icon.source: "/images/icons/copy.png"

            onTriggered: {
                backend.setClipboardText(tagContextMenu.tag)
            }
        }
    }

    ListView {
        clip: true
        anchors.fill: parent
        anchors.margins: 8

        model: image.tags

        section.property: "type"
        section.criteria: ViewSection.FullString
        section.delegate: Text {
            text: section in globals.tagTypes ? globals.tagTypes[section] : section
            height: 34
            topPadding: 10
            font.bold: true
            font.pixelSize: 14
        }

        delegate: RowLayout {
            property string tag: model.name
            property string tagColor: settings.value("Coloring/Colors/" + model.type + "s", null)

            width: parent.width
            height: 34
            spacing: 0

            Label {
                text: tag
                color: tagColor
                Layout.fillHeight: true
                Layout.fillWidth: true
                topPadding: 5

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        root.closed()
                        searchTab.load(tag)
                    }
                    onPressAndHold: {
                        tagContextMenu.tag = tag
                        tagContextMenu.popup()
                    }
                }
            }

            ToolButton {
                property bool isInQuery: pageLoader.query.split(' ').indexOf(tag) >= 0

                icon.source: "/images/icons/" + (!isInQuery ? "add" : "remove") + ".png"
                Layout.fillHeight: true
                Layout.preferredWidth: 34

                onClicked: {
                    root.closed()
                    if (!isInQuery) {
                        searchTab.load(pageLoader.query + ' ' + tag)
                    } else {
                        searchTab.load(pageLoader.query.split(' ').filter(t => t !== tag).join(' '))
                    }
                }
            }

            ToolButton {
                property bool isInQuery: pageLoader.query.split(' ').indexOf('-' + tag) >= 0

                icon.source: "/images/icons/" + (!isInQuery ? "remove" : "add") + ".png"
                Layout.fillHeight: true
                Layout.preferredWidth: 34

                onClicked: {
                    root.closed()
                    if (!isInQuery) {
                        searchTab.load(pageLoader.query + ' -' + tag)
                    } else {
                        searchTab.load(pageLoader.query.split(' ').filter(t => t !== '-' + tag).join(' '))
                    }
                }
            }

            ToolButton {
                icon.source: "/images/icons/menu.png"
                Layout.fillHeight: true
                Layout.preferredWidth: 34

                onClicked: {
                    tagContextMenu.tag = tag
                    tagContextMenu.popup()
                }
            }
        }
    }
}
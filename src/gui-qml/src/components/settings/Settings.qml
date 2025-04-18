import QtQuick 2.12
import QtQuick.Controls.Material 2.12
import Qt.labs.platform 1.1

Item {
    id: root

    property var obj: settings

    property Setting language: Setting {
        key: "language"
        def: "English"
        obj: root.obj

        onChanged: languageLoader.setLanguage(value)
    }
    property Setting resultsInfiniteScroll: Setting {
        key: "infiniteScroll"
        def: "disabled"
        parser: (val) => val === "scroll"
        writer: (val) => val ? "scroll" : "disabled"
    }
    property Setting resultsColumnCountPortrait: Setting {
        key: "resultsColumnCountPortrait"
        def: 3
        obj: root.obj
    }
    property Setting resultsColumnCountLandscape: Setting {
        key: "resultsColumnCountLandscape"
        def: 5
        obj: root.obj
    }
    property Setting resultsLayoutType: Setting {
        key: "resultsLayoutType"
        def: "flow"
        obj: root.obj
    }
    property Setting resultsSpaceBetweenImages: Setting {
        key: "resultsSpaceBetweenImages"
        def: "minimal"
        obj: root.obj
    }
    property Setting resultsHeightToWidthRatio: Setting {
        key: "resultsHeightToWidthRatio"
        def: 1
        obj: root.obj
    }
    property Setting resultsThumbnailFillMode: Setting {
        key: "resultsThumbnailFillMode"
        def: "crop"
        obj: root.obj
    }
    property Setting resultsRoundImages: Setting {
        key: "resultsRoundImages"
        def: false
        obj: root.obj
    }
    property Setting save_filename: Setting {
        key: "Save/filename"
        def: "%md5%.%ext%"
        obj: root.obj
    }
    property Setting save_path: Setting {
        key: "Save/path"
        def: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0] + "/Grabber"
        obj: root.obj
    }
    property Setting save_separator: Setting {
        key: "Save/separator"
        def: " "
        obj: root.obj
    }
    property Setting save_replaceblanks: Setting {
        key: "Save/replaceblanks"
        def: false
        obj: root.obj
    }
    property Setting save_noJpeg: Setting {
        key: "Save/noJpeg"
        def: true
        obj: root.obj
    }
    property Setting save_md5Duplicates: Setting {
        key: "Save/md5Duplicates"
        def: "save"
        obj: root.obj
    }
    property Setting save_md5DuplicatesSameDir: Setting {
        key: "Save/md5DuplicatesSameDir"
        def: "save"
        obj: root.obj
    }
    property Setting save_keepDeletedMd5: Setting {
        key: "Save/keepDeletedMd5"
        def: false
        obj: root.obj
    }
    property Setting globalAddedTags: Setting {
        key: "add"
        def: ""
        obj: root.obj
    }
    property Setting globalPostFilters: Setting {
        key: "globalPostFilter"
        def: ""
        obj: root.obj
    }
    property var blacklist: Item {
        property string value: backend.getBlacklist()
        function setValue(val) {
            backend.setBlacklist(val)
            value = val
        }
    }
    property Setting hideblacklisted: Setting {
        key: "hideblacklisted"
        def: false
        obj: root.obj
    }
    property Setting downloadblacklist: Setting {
        key: "downloadblacklist"
        def: false
        obj: root.obj
    }
    property Setting removedTags: Setting {
        key: "ignoredtags"
        def: ""
        obj: root.obj
    }
    property var ignoredTags: Item {
        property string value: backend.getIgnored()
        function setValue(val) {
            backend.setIgnored(val)
        }
    }
    property Setting viewer_viewSamples: Setting {
        key: "Viewer/viewSamples"
        def: true // false on desktop
        obj: root.obj
    }
    property Setting checkForUpdates: Setting {
        key: "check_for_updates"
        def: 24 * 60 * 60
        obj: root.obj
    }

    // QML-only settings
    property Setting viewer_buttonsAtBottom: Setting {
        key: "Viewer/buttonsAtBottom"
        def: false
        obj: root.obj
    }
    property Setting activeSource: Setting {
        key: "activeSource"
        def: "safebooru.org"
        obj: root.obj
    }

    // API order
    property Setting source1: Setting {
        key: "source_1"
        def: "xml"
        obj: root.obj
    }
    property Setting source2: Setting {
        key: "source_2"
        def: "json"
        obj: root.obj
    }
    property Setting source3: Setting {
        key: "source_3"
        def: "regex"
        obj: root.obj
    }
    property Setting source4: Setting {
        key: "source_4"
        def: "rss"
        obj: root.obj
    }

    // Proxy
    property Setting proxy_use: Setting {
        key: "Proxy/use"
        def: false
        obj: root.obj
    }
    property Setting proxy_useSystem: Setting {
        key: "Proxy/useSystem"
        def: false
        obj: root.obj
    }
    property Setting proxy_type: Setting {
        key: "Proxy/type"
        def: "http"
        obj: root.obj
    }
    property Setting proxy_hostName: Setting {
        key: "Proxy/hostName"
        def: ""
        obj: root.obj
    }
    property Setting proxy_port: Setting {
        key: "Proxy/port"
        def: ""
        obj: root.obj
    }
    property Setting proxy_user: Setting {
        key: "Proxy/user"
        def: ""
        obj: root.obj
    }
    property Setting proxy_password: Setting {
        key: "Proxy/password"
        def: ""
        obj: root.obj
    }

    // Appearance
    property Setting appearance_materialTheme: Setting {
        key: "Appearance/materialTheme"
        def: Material.System
        obj: root.obj
        parser: (v) => globals.materialThemes[Number(v)]
    }
    property Setting appearance_materialPrimary: Setting {
        key: "Appearance/materialPrimary"
        def: Material.Blue
        obj: root.obj
        parser: (v) => globals.materialColors[Number(v)]
    }
    property Setting appearance_materialAccent: Setting {
        key: "Appearance/materialAccent"
        def: Material.Amber
        obj: root.obj
        parser: (v) => globals.materialColors[Number(v)]
    }
    property Setting imageBackgroundColor: Setting {
        key: "imageBackgroundColor"
        def: ""
        obj: root.obj
    }

    // Coloring
    property Setting coloring_colors_artists: Setting {
        key: "Coloring/Colors/artists"
        def: ""
        obj: root.obj
    }
    property Setting coloring_colors_circles: Setting {
        key: "Coloring/Colors/circles"
        def: ""
        obj: root.obj
    }
    property Setting coloring_colors_copyrights: Setting {
        key: "Coloring/Colors/copyrights"
        def: ""
        obj: root.obj
    }
    property Setting coloring_colors_characters: Setting {
        key: "Coloring/Colors/characters"
        def: ""
        obj: root.obj
    }
    property Setting coloring_colors_species: Setting {
        key: "Coloring/Colors/species"
        def: ""
        obj: root.obj
    }
    property Setting coloring_colors_metas: Setting {
        key: "Coloring/Colors/metas"
        def: ""
        obj: root.obj
    }
    property Setting coloring_colors_models: Setting {
        key: "Coloring/Colors/models"
        def: ""
        obj: root.obj
    }
    property Setting coloring_colors_generals: Setting {
        key: "Coloring/Colors/generals"
        def: ""
        obj: root.obj
    }
    property Setting coloring_colors_favorites: Setting {
        key: "Coloring/Colors/favorites"
        def: ""
        obj: root.obj
    }
    property Setting coloring_colors_keptForLater: Setting {
        key: "Coloring/Colors/keptForLater"
        def: ""
        obj: root.obj
    }
    property Setting coloring_colors_blacklisteds: Setting {
        key: "Coloring/Colors/blacklisteds"
        def: ""
        obj: root.obj
    }
    property Setting coloring_colors_ignoreds: Setting {
        key: "Coloring/Colors/ignoreds"
        def: ""
        obj: root.obj
    }

    // Mobile-specific settings
    property Setting mobile_confirmExit: Setting {
        key: "Mobile/confirmExit"
        def: true
        obj: root.obj
    }
    property Setting mobile_doubleBackExit: Setting {
        key: "Mobile/doubleBackExit"
        def: true
        obj: root.obj
    }

    // Setting to gate V8 features
    property Setting v8: Setting {
        key: "v8"
        def: false
        obj: root.obj
    }
}

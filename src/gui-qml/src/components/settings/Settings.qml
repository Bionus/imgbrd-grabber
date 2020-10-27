import QtQuick 2.12
import Qt.labs.platform 1.1

Item {
    id: root

    property var obj: settings

    property Setting language: Setting {
        key: "language"
        def: "English"
        obj: root.obj
    }
    property Setting resultsColumnCount: Setting {
        key: "resultsColumnCount"
        def: 3
        obj: root.obj
    }
    property Setting save_filename: Setting {
        key: "Save/filename"
        def: "%md5%.%ext%"
        obj: root.obj
    }
    property Setting save_path: Setting {
        key: "Save/path"
        def: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
        obj: root.obj
    }
    property Setting hideblacklisted: Setting {
        key: "hideblacklisted"
        def: true
        obj: root.obj
    }
}

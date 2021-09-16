import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import Qt.labs.platform 1.1

import "./items"

Page {
    id: root

    signal closed()

    property var site

    property var headers: []
    function refreshHeaders() {
        var arr = []
        for (var key of root.site.settings.childKeys("Headers")) {
            var value = root.site.settings.value("Headers/" + key)
            arr.push({ key, value })
        }
        headers = arr
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                icon.source: "/images/icons/back.png"
                onClicked: root.closed()
            }

            Label {
                text: root.site.name || root.site.url
                elide: Label.ElideRight
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }

            ToolButton {
                visible: image.sampleUrl !== image.fileUrl
                icon.source: "/images/icons/delete.png"
                onClicked: {
                    if (backend.removeSite(root.site)) {
                        root.closed();
                    } else {
                        console.error("Error removing the site")
                    }
                }
            }
        }
    }

    ScrollView {
        width: parent.width
        height: parent.height
        contentWidth: column.width
        contentHeight: column.height
        clip: true

        ColumnLayout {
            id: column
            width: parent.width
            spacing: 0

            SettingTitle {
                Layout.fillWidth: true
                text: qsTr("General")
            }
            TextFieldSetting {
                name: qsTr("Name")
                setting: Setting {
                    key: "name"
                    def: root.site.url
                    obj: root.site.settings
                }
                Layout.fillWidth: true
            }
            CheckBoxSetting {
                name: qsTr("HTTPS")
                subtitle: qsTr("Use a secure connection.")
                setting: Setting {
                    key: "ssl"
                    def: false
                    obj: root.site.settings
                }
                Layout.fillWidth: true
            }

            SettingTitle {
                Layout.fillWidth: true
                text: qsTr("Login")
            }
            Setting {
                id: loginTypeSetting
                key: "login/type"
                def: "url"
                obj: root.site.settings
            }
            ComboSetting {
                name: qsTr("Type")
                options: root.site.authFields.map(f => globals.authTypes[f.type] || f.type)
                values: root.site.authFields.map(f => f.type)
                setting: loginTypeSetting
                Layout.fillWidth: true
            }
            Repeater {
                model: root.site.authFields
                Layout.fillWidth: true

                delegate: Item {
                    height: 100
                    width: parent.width
                    visible: modelData.type === loginTypeSetting.value

                    Column {
                        spacing: 0
                        width: parent.width

                        Repeater {
                            model: modelData.fields
                            width: parent.width

                            delegate: TextFieldSetting {
                                name: globals.authFieldLabels[modelData.id] || modelData.id
                                setting: Setting {
                                    key: "auth/" + modelData.id
                                    def: modelData.def
                                    obj: root.site.settings
                                }
                                echoMode: modelData.isPassword ? TextInput.Password : TextInput.Normal
                                width: parent.width
                            }
                        }
                    }
                }
            }

            SettingTitle {
                Layout.fillWidth: true
                text: qsTr("API order")
            }
            Setting {
                id: useDefaultApiOrderSetting
                key: "sources/usedefault"
                def: true
                obj: root.site.settings
            }
            CheckBoxSetting {
                name: qsTr("Use default API order")
                setting: useDefaultApiOrderSetting
                Layout.fillWidth: true
            }
            ComboSetting {
                name: qsTr("Source 1")
                options: ["", ...globals.apiTypes]
                values: ["", ...globals.apiTypesKeys]
                setting: Setting {
                    key: "sources/source_1"
                    def: gSettings.source1.value
                    obj: root.site.settings
                }
                Layout.fillWidth: true
                visible: !useDefaultApiOrderSetting.value
            }
            ComboSetting {
                name: qsTr("Source 2")
                options: ["", ...globals.apiTypes]
                values: ["", ...globals.apiTypesKeys]
                setting: Setting {
                    key: "sources/source_2"
                    def: gSettings.source2.value
                    obj: root.site.settings
                }
                Layout.fillWidth: true
                visible: !useDefaultApiOrderSetting.value
            }
            ComboSetting {
                name: qsTr("Source 3")
                options: ["", ...globals.apiTypes]
                values: ["", ...globals.apiTypesKeys]
                setting: Setting {
                    key: "sources/source_3"
                    def: gSettings.source3.value
                    obj: root.site.settings
                }
                Layout.fillWidth: true
                visible: !useDefaultApiOrderSetting.value
            }
            ComboSetting {
                name: qsTr("Source 4")
                options: ["", ...globals.apiTypes]
                values: ["", ...globals.apiTypesKeys]
                setting: Setting {
                    key: "sources/source_4"
                    def: gSettings.source4.value
                    obj: root.site.settings
                }
                Layout.fillWidth: true
                visible: !useDefaultApiOrderSetting.value
            }

            SettingTitle {
                Layout.fillWidth: true
                text: qsTr("Download")
            }
            // FIXME(Bionus): Not supported by the AsyncImageProvider at the moment
            /*SpinBoxSetting {
                name: qsTr("Interval (thumbnail)")
                max: 60 * 60
                setting: Setting {
                    key: "download/throttle_thumbnail"
                    def: 0
                    obj: root.site.settings
                }
                Layout.fillWidth: true
            }*/
            SpinBoxSetting {
                name: qsTr("Interval (image)")
                max: 60 * 60
                setting: Setting {
                    key: "download/throttle_image"
                    def: 0
                    obj: root.site.settings
                }
                Layout.fillWidth: true
            }
            SpinBoxSetting {
                name: qsTr("Interval (page)")
                max: 60 * 60
                setting: Setting {
                    key: "download/throttle_page"
                    def: 0
                    obj: root.site.settings
                }
                Layout.fillWidth: true
            }
            SpinBoxSetting {
                name: qsTr("Interval (details)")
                max: 60 * 60
                setting: Setting {
                    key: "download/throttle_details"
                    def: 0
                    obj: root.site.settings
                }
                Layout.fillWidth: true
            }
            SpinBoxSetting {
                name: qsTr("Interval (error)")
                max: 60 * 60
                setting: Setting {
                    key: "download/throttle_retry"
                    def: 0
                    obj: root.site.settings
                }
                Layout.fillWidth: true
            }

            SettingTitle {
                Layout.fillWidth: true
                text: qsTr("Headers")
            }
            KeyValueSetting {
                values: headers
                onAppend: {
                    root.site.settings.setValue("Headers/" + key, value)
                    refreshHeaders()
                }
                onEdit: {
                    root.site.settings.remove("Headers/" + oldKey)
                    root.site.settings.setValue("Headers/" + key, value)
                    refreshHeaders()
                }
                onRemove: {
                    root.site.settings.remove("Headers/" + key)
                    refreshHeaders()
                }
                Layout.fillWidth: true
                Component.onCompleted: refreshHeaders()
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }
}

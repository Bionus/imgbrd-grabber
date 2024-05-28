// Copyright (C) 2020 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick

/*
   A cross-graphics API implementation of QtGraphicalEffects' OpacityMask.
 */
Item {
    id: rootItem

    property variant source
    property variant maskSource
    property bool cached: false

    ShaderEffectSource {
        id: maskSourceProxy
        sourceItem: maskSource
        hideSource: false
        smooth: true
        visible: false
    }

    ShaderEffectSource {
        id: cacheItem
        anchors.fill: parent
        visible: rootItem.cached
        smooth: true
        sourceItem: shaderItem
        live: true
        hideSource: visible
    }

    ShaderEffect {
        id: shaderItem
        property variant source: rootItem.source
        property variant maskSource: maskSourceProxy

        anchors.fill: parent

        fragmentShader: "qrc:/src/vendor/OpacityMask.frag.qsb"
    }
}

/*
 * This file is part of Fluid.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2018 Michael Spencer <sonrisesoftware@gmail.com>
 *
 * $BEGIN_LICENSE:MPL2$
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * $END_LICENSE$
 */

import QtQuick 2.10

/*!
    \qmltype ColumnFlow
    \inqmlmodule Fluid.Layouts
    \ingroup fluidlayouts

    \brief Automatically position children in columns.

    \code
    import QtQuick 2.10
    import Fluid.Layouts 1.0 as FluidLayouts

    Item {
        width: 600
        height: 600

        FluidLayouts.ColumnFlow {
            anchors.fill: parent
            columns: 5
            model: 20

            delegate: Rectangle {
                id: item
                height: 100.0 * Math.random()
                color: Qt.rgba(Math.random(), Math.random(), Math.random(), Math.random())
                Text {
                    text: index
                }
            }
        }
    }
    \endcode
*/
Item {
    id: columnFlow

    /*!
        \qmlproperty int columnWidth

        Column width.
        This property is \c 100 by default.
    */
    property int columnWidth: 100

    /*!
        \qmlproperty int columns

        Number of columns.
        By default it fits as many columns as possible.
    */
    property int columns: Math.max(0, Math.floor(width / columnWidth))

    /*!
        \qmlproperty any model

        The model providing data to the column flow.

        This property can be set to any of the supported \l {qml-data-models}{data models}.

        \sa Repeater::model
    */
    property alias model: repeater.model

    /*!
        \qmlproperty Component delegate

        The delegate provides a template defining each item istantiated by the column flow.

        \sa Repeater::delegate
    */
    property alias delegate: repeater.delegate

    /*!
        \qmlproperty int contentHeight

        Content height.
    */
    property int contentHeight: 0

    /*!
        \qmlproperty bool repeaterCompleted

        This property holds whether the layout is done.
    */
    readonly property alias repeaterCompleted: __private.repeaterCompleted

    height: contentHeight

    onColumnsChanged: reEvalColumns()
    onModelChanged: reEvalColumns()

    onWidthChanged: updateWidths()

    QtObject {
        id: __private

        property bool repeaterCompleted: false
    }

    /*!
        \qmlmethod void ColumnFlow::updateWidths()

        Set the width of all delegates.
    */
    function updateWidths() {
        if (repeaterCompleted) {
            var count = 0;

            // Add the first <column> elements
            for (var i = 0; count < columns && i < columnFlow.children.length; i++) {
                if (!columnFlow.children[i] || String(columnFlow.children[i]).indexOf("QQuickRepeater") == 0)
                    continue;

                columnFlow.children[i].width = width / columns;
                count++;
            }
        }
    }

    /*!
        \qmlmethod void ColumnFlow::reEvalColumns()

        Relayout the columns.
    */
    function reEvalColumns() {
        if (!repeaterCompleted)
            return;
        var i, j
        var columnHeights = new Array(columns);
        var lastItem = new Array(columns);
        var lastI = -1;
        var count = 0;

        // Add the first <column> elements
        for (i = 0; count < columns && i < columnFlow.children.length; i++) {
            if (!columnFlow.children[i] || String(columnFlow.children[i]).indexOf("QQuickRepeater") == 0
                    || !columnFlow.children[i].visible)
                continue;

            lastItem[count] = i;
            columnHeights[count] = columnFlow.children[i].height;

            columnFlow.children[i].anchors.top = columnFlow.top;
            columnFlow.children[i].anchors.left = (lastI === -1 ? columnFlow.left : columnFlow.children[lastI].right);
            columnFlow.children[i].anchors.right = undefined;
            columnFlow.children[i].width = columnFlow.width / columns;

            lastI = i;
            count++;
        }

        // Add the other elements
        for (i = i; i < columnFlow.children.length; i++) {
            var highestHeight = Number.MAX_VALUE;
            var newColumn = 0;

            if (!columnFlow.children[i] || !columnFlow.children[i].visible)
                continue;

            // find the shortest column
            for (j = 0; j < columns; j++) {
                if (columnHeights[j] < highestHeight) {
                    newColumn = j;
                    highestHeight = columnHeights[j];
                }
            }

            // add the element to the shortest column
            columnFlow.children[i].anchors.top = columnFlow.children[lastItem[newColumn]].bottom;
            columnFlow.children[i].anchors.left = columnFlow.children[lastItem[newColumn]].left;
            columnFlow.children[i].anchors.right = columnFlow.children[lastItem[newColumn]].right;

            lastItem[newColumn] = i;
            columnHeights[newColumn] += columnFlow.children[i].height;
        }

        var cHeight = 0;
        for (i = 0; i < columns; i++) {
            if (!columnHeights[i])
                continue;
            cHeight = Math.max(cHeight, columnHeights[i]);
        }
        contentHeight = cHeight;

        updateWidths();
    }

    Repeater {
        id: repeater
        model: columnFlow.model

        Component.onCompleted: {
            __private.repeaterCompleted = true;
            columnFlow.reEvalColumns();
        }
    }
}

/****************************************************************************
 **
 ** Copyright (C) 2016 Ivan Vizir <define-true-false@yandex.com>
 ** Contact: https://www.qt.io/licensing/
 **
 ** This file is part of the QtWinExtras module of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and The Qt Company. For licensing terms
 ** and conditions see https://www.qt.io/terms-conditions. For further
 ** information use the contact form at https://www.qt.io/contact-us.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 3 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL3 included in the
 ** packaging of this file. Please review the following information to
 ** ensure the GNU Lesser General Public License version 3 requirements
 ** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 2.0 or (at your option) the GNU General
 ** Public license version 3 or any later version approved by the KDE Free
 ** Qt Foundation. The licenses are as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file. Please review the following
 ** information to ensure the GNU General Public License requirements will
 ** be met: https://www.gnu.org/licenses/gpl-2.0.html and
 ** https://www.gnu.org/licenses/gpl-3.0.html.
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/
#include "windowsguidsdefs_p.h"
#include "winpropkey_p.h"
#include "winshobjidl_p.h"
QT_BEGIN_NAMESPACE
// Some of these symbols are defined to varying extents in the various
// MinGW versions. To avoid potential conflicts, we provide them prefixed with
// 'q' in Qt's namespace.
const GUID qCLSID_DestinationList                             = {0x77f10cf0, 0x3db5, 0x4966, {0xb5,0x20,0xb7,0xc5,0x4f,0xd3,0x5e,0xd6}};
const GUID qCLSID_EnumerableObjectCollection                  = {0x2d3468c1, 0x36a7, 0x43b6, {0xac,0x24,0xd3,0xf0,0x2f,0xd9,0x60,0x7a}};
const GUID qIID_ICustomDestinationList                        = {0x6332debf, 0x87b5, 0x4670, {0x90,0xc0,0x5e,0x57,0xb4,0x08,0xa4,0x9e}};
const GUID qIID_IApplicationDestinations                      = {0x12337d35, 0x94c6, 0x48a0, {0xbc,0xe7,0x6a,0x9c,0x69,0xd4,0xd6,0x00}};
const GUID qCLSID_ApplicationDestinations                     = {0x86c14003, 0x4d6b, 0x4ef3, {0xa7,0xb4,0x05,0x06,0x66,0x3b,0x2e,0x68}};
const GUID qIID_IApplicationDocumentLists                     = {0x3c594f9f, 0x9f30, 0x47a1, {0x97,0x9a,0xc9,0xe8,0x3d,0x3d,0x0a,0x06}};
const GUID qCLSID_ApplicationDocumentLists                    = {0x86bec222, 0x30f2, 0x47e0, {0x9f,0x25,0x60,0xd1,0x1c,0xd7,0x5c,0x28}};
const GUID qIID_IObjectArray                                  = {0x92ca9dcd, 0x5622, 0x4bba, {0xa8,0x05,0x5e,0x9f,0x54,0x1b,0xd8,0xc9}};
const GUID qIID_IObjectCollection                             = {0x5632b1a4, 0xe38a, 0x400a, {0x92,0x8a,0xd4,0xcd,0x63,0x23,0x02,0x95}};
const GUID qIID_IPropertyStore                                = {0x886d8eeb, 0x8cf2, 0x4446, {0x8d,0x02,0xcd,0xba,0x1d,0xbd,0xcf,0x99}};
const GUID qIID_ITaskbarList3                                 = {0xea1afb91, 0x9e28, 0x4b86, {0x90,0xe9,0x9e,0x9f,0x8a,0x5e,0xef,0xaf}};
const GUID qIID_ITaskbarList4                                 = {0xc43dc798, 0x95d1, 0x4bea, {0x90,0x30, 0xbb,0x99,0xe2,0x98,0x3a,0x1a}};
const PROPERTYKEY qPKEY_Title                                 = {{0xf29f85e0, 0x4ff9, 0x1068, {0xab,0x91,0x08,0x00,0x2b,0x27,0xb3,0xd9}}, 2};
const PROPERTYKEY qPKEY_Link_Arguments                        = {{0x436f2667, 0x14e2, 0x4feb, {0xb3,0x0a,0x14,0x6c,0x53,0xb5,0xb6,0x74}}, 100};
const PROPERTYKEY qPKEY_AppUserModel_IsDestListSeparator      = {{0x9f4c2855, 0x9f79, 0x4b39, {0xa8,0xd0,0xe1,0xd4,0x2d,0xe1,0xd5,0xf3}}, 6};
const GUID qIID_IShellItem2                                   = {0x7e9fb0d3, 0x919f, 0x4307, {0xab,0x2e,0x9b,0x18,0x60,0x31,0x0c,0x93}};
const GUID qIID_IShellLinkW                                   = {0x000214f9, 0x0000, 0x0000, {0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};
const GUID qIID_ITaskbarList                                  = {0xea1afb91, 0x9e28, 0x4b86, {0x90,0xe9,0x9e,0x9f,0x8a,0x5e,0xef,0xaf}};
const GUID qIID_ITaskbarList2                                 = {0x602d4995, 0xb13a, 0x429b, {0xa6,0x6e,0x19,0x35,0xe4,0x4f,0x43,0x17}};
const GUID qIID_IUnknown                                      = {0x00000000, 0x0000, 0x0000, {0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};
const GUID qGUID_NULL                                         = {0x00000000, 0x0000, 0x0000, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
QT_END_NAMESPACE
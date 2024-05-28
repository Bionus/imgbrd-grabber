/****************************************************************************
 **
 ** Copyright (C) 2016 Ivan Vizir <define-true-false@yandex.com>
 ** Copyright (C) 2016 The Qt Company Ltd.
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
#include "qwintaskbarbutton.h"
#include "qwintaskbarbutton_p.h"
#include "qwintaskbarprogress.h"
#include "qwinfunctions.h"
#include "qwinfunctions_p.h"
#include "qwineventfilter_p.h"
#include "qwinevent.h"
#include "winshobjidl_p.h"
#include "windowsguidsdefs_p.h"
#include <QtGui/qwindow.h>
#include <QtGui/qicon.h>
#include <QtCore/qpair.h>
#include <dwmapi.h>
#include <shobjidl.h>
QT_BEGIN_NAMESPACE
/*!
    \class QWinTaskbarButton
    \inmodule QtWinExtras
    \brief The QWinTaskbarButton class represents the Windows taskbar button for
    a top-level window (Windows 7 and newer).
    \since 5.2
    The QWinTaskbarButton class enables you to set overlay icons on a taskbar
    button, and provides access to its progress indicator.
    An overlay icon indicates change in the state of an application, whereas
    a progress indicator shows how time-consuming tasks are progressing.
    \image taskbar-button.png Taskbar Button
    The following example code illustrates how to use the QWinTaskbarButton
    and QWinTaskbarProgress classes to adjust the look of the taskbar button:
    \snippet code/taskbar.cpp taskbar_cpp
    \note QWidget::windowHandle() returns a valid instance of a QWindow only
    after the widget has been shown. It is therefore recommended to delay the
    initialization of the QWinTaskbarButton instances until QWidget::showEvent().
    \note The class wraps API only available since Windows 7. Instantiating it
    on Windows XP or Windows Vista causes a runtime warning.
    \sa QWinTaskbarProgress
 */
static TBPFLAG nativeProgressState(QWinTaskbarProgress *progress)
{
    if (!progress || !progress->isVisible())
        return TBPF_NOPROGRESS;
    if (progress->isStopped())
        return TBPF_ERROR;
    if (progress->isPaused())
        return TBPF_PAUSED;
    if (progress->minimum() == 0 && progress->maximum() == 0)
        return TBPF_INDETERMINATE;
    return TBPF_NORMAL;
}
QWinTaskbarButtonPrivate::QWinTaskbarButtonPrivate()
{
    HRESULT hresult = CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_INPROC_SERVER, qIID_ITaskbarList4, reinterpret_cast<void **>(&pTbList));
    if (FAILED(hresult)) {
        pTbList = nullptr;
        const QString err = QtWin::errorStringFromHresult(hresult);
        qWarning("QWinTaskbarButton: qIID_ITaskbarList4 was not created: %#010x, %s.", unsigned(hresult), qPrintable(err));
    } else if (FAILED(pTbList->HrInit())) {
        pTbList->Release();
        pTbList = nullptr;
        const QString err = QtWin::errorStringFromHresult(hresult);
        qWarning("QWinTaskbarButton: qIID_ITaskbarList4 was not initialized: %#010x, %s.", unsigned(hresult), qPrintable(err));
    }
}
QWinTaskbarButtonPrivate::~QWinTaskbarButtonPrivate()
{
    if (pTbList)
        pTbList->Release();
}
HWND QWinTaskbarButtonPrivate::handle()
{
    return reinterpret_cast<HWND>(window->winId());
}
int QWinTaskbarButtonPrivate::iconSize() const
{
    return GetSystemMetrics(SM_CXSMICON);
}
void QWinTaskbarButtonPrivate::updateOverlayIcon()
{
    if (!pTbList || !window)
        return;
    wchar_t *descrPtr = nullptr;
    HICON hicon = nullptr;
    if (!overlayAccessibleDescription.isEmpty())
        descrPtr = qt_qstringToNullTerminated(overlayAccessibleDescription);
    if (!overlayIcon.isNull())
        hicon = QtWin::toHICON(overlayIcon.pixmap(iconSize()));
    if (hicon)
        pTbList->SetOverlayIcon(handle(), hicon, descrPtr);
    else if (!hicon && !overlayIcon.isNull())
        pTbList->SetOverlayIcon(handle(), static_cast<HICON>(LoadImage(nullptr, IDI_APPLICATION, IMAGE_ICON, SM_CXSMICON, SM_CYSMICON, LR_SHARED)), descrPtr);
    else
        pTbList->SetOverlayIcon(handle(), nullptr, descrPtr);
    if (hicon)
        DestroyIcon(hicon);
    delete[] descrPtr;
}
void QWinTaskbarButtonPrivate::_q_updateProgress()
{
    if (!pTbList || !window)
        return;
    if (progressBar) {
        const int min = progressBar->minimum();
        const int max = progressBar->maximum();
        const int range = max - min;
        if (range > 0) {
            const int value = qRound(double(100) * (double(progressBar->value() - min)) / double(range));
            pTbList->SetProgressValue(handle(), ULONGLONG(value), 100);
        }
    }
    pTbList->SetProgressState(handle(), nativeProgressState(progressBar));
}
/*!
    Constructs a QWinTaskbarButton with the specified \a parent.
    If \a parent is an instance of QWindow, it is automatically
    assigned as the taskbar button's \l window.
 */
QWinTaskbarButton::QWinTaskbarButton(QObject *parent) :
    QObject(parent), d_ptr(new QWinTaskbarButtonPrivate)
{
    QWinEventFilter::setup();
    setWindow(qobject_cast<QWindow *>(parent));
}
/*!
    Destroys the QWinTaskbarButton.
 */
QWinTaskbarButton::~QWinTaskbarButton() = default;
/*!
    \property QWinTaskbarButton::window
    \brief the window whose taskbar button is manipulated
 */
void QWinTaskbarButton::setWindow(QWindow *window)
{
    Q_D(QWinTaskbarButton);
    if (d->window)
        d->window->removeEventFilter(this);
    d->window = window;
    if (d->window) {
        d->window->installEventFilter(this);
        if (d->window->isVisible()) {
            d->_q_updateProgress();
            d->updateOverlayIcon();
        }
    }
}
QWindow *QWinTaskbarButton::window() const
{
    Q_D(const QWinTaskbarButton);
    return d->window;
}
/*!
    \property QWinTaskbarButton::overlayIcon
    \brief the overlay icon of the taskbar button
 */
QIcon QWinTaskbarButton::overlayIcon() const
{
    Q_D(const QWinTaskbarButton);
    return d->overlayIcon;
}
void QWinTaskbarButton::setOverlayIcon(const QIcon &icon)
{
    Q_D(QWinTaskbarButton);
    d->overlayIcon = icon;
    d->updateOverlayIcon();
}
void QWinTaskbarButton::clearOverlayIcon()
{
    setOverlayAccessibleDescription(QString());
    setOverlayIcon(QIcon());
}
/*!
    \property QWinTaskbarButton::overlayAccessibleDescription
    \brief the description of the overlay for accessibility purposes
    \sa overlayIcon
 */
QString QWinTaskbarButton::overlayAccessibleDescription() const
{
    Q_D(const QWinTaskbarButton);
    return d->overlayAccessibleDescription;
}
void QWinTaskbarButton::setOverlayAccessibleDescription(const QString &description)
{
    Q_D(QWinTaskbarButton);
    d->overlayAccessibleDescription = description;
    d->updateOverlayIcon();
}
/*!
    \property QWinTaskbarButton::progress
    \brief the progress indicator of the taskbar button
    \note The progress indicator is not \l{QWinTaskbarProgress::visible}{visible} by default.
 */
QWinTaskbarProgress *QWinTaskbarButton::progress() const
{
    Q_D(const QWinTaskbarButton);
    if (!d->progressBar) {
        auto *that = const_cast<QWinTaskbarButton *>(this);
        auto *pbar = new QWinTaskbarProgress(that);
        connect(pbar, SIGNAL(destroyed()), this, SLOT(_q_updateProgress()));
        connect(pbar, SIGNAL(valueChanged(int)), this, SLOT(_q_updateProgress()));
        connect(pbar, SIGNAL(minimumChanged(int)), this, SLOT(_q_updateProgress()));
        connect(pbar, SIGNAL(maximumChanged(int)), this, SLOT(_q_updateProgress()));
        connect(pbar, SIGNAL(visibilityChanged(bool)), this, SLOT(_q_updateProgress()));
        connect(pbar, SIGNAL(pausedChanged(bool)), this, SLOT(_q_updateProgress()));
        connect(pbar, SIGNAL(stoppedChanged(bool)), this, SLOT(_q_updateProgress()));
        that->d_func()->progressBar = pbar;
        that->d_func()->_q_updateProgress();
    }
    return d->progressBar;
}
/*!
    \internal
    Intercepts TaskbarButtonCreated messages.
 */
bool QWinTaskbarButton::eventFilter(QObject *object, QEvent *event)
{
    Q_D(QWinTaskbarButton);
    if (object == d->window && event->type() == QWinEvent::TaskbarButtonCreated) {
        d->_q_updateProgress();
        d->updateOverlayIcon();
    }
    return false;
}
QT_END_NAMESPACE
#include "moc_qwintaskbarbutton.cpp"
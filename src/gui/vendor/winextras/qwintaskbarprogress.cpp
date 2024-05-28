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
#include "qwintaskbarprogress.h"
QT_BEGIN_NAMESPACE
/*!
    \class QWinTaskbarProgress
    \inmodule QtWinExtras
    \brief The QWinTaskbarProgress class represents a progress indicator in the Windows taskbar.
    \since 5.2
    A progress indicator is used to give the user an indication of the progress
    of an operation and to reassure them that the application is still running.
    The progress indicator uses the concept of \e steps. It is set up by specifying
    the minimum and maximum possible step values, and it will display the percentage
    of steps that have been completed when you later give it the current step value.
    The percentage is calculated by dividing the progress (value() - minimum())
    divided by maximum() - minimum().
    The minimum and maximum number of steps can be specified by calling setMinimum()
    and setMaximum(). The current number of steps is set with setValue(). The progress
    indicator can be rewound to the beginning with reset().
    If minimum and maximum both are set to \c 0, the indicator shows up as a busy
    (indeterminate) indicator instead of a percentage of steps. This is useful when
    it is not possible to determine the number of steps.
    \table
    \row \li \inlineimage taskbar-progress.png Screenshot of a progress indicator
         \li A progress indicator at 50%.
    \row \li \inlineimage taskbar-progress-paused.png Screenshot of a paused progress indicator
         \li A paused progress indicator at 50%.
    \row \li \inlineimage taskbar-progress-stopped.png Screenshot of a stopped progress indicator
        \li A stopped progress indicator at 50%.
    \row \li \inlineimage taskbar-progress-indeterminate.png Screenshot of an indeterminate progress indicator
         \li An indeterminate progress indicator.
    \endtable
    \note The final appearance of the progress indicator varies depending on the active Windows theme.
    \sa QWinTaskbarButton
 */
/*!
    \fn void QWinTaskbarProgress::pausedChanged(bool paused)
    \internal (for QWinTaskbarButton and QML compatibility)
 */
/*!
    \fn void QWinTaskbarProgress::stoppedChanged(bool stopped)
    \internal (for QWinTaskbarButton and QML compatibility)
 */
class QWinTaskbarProgressPrivate
{
public:
    int value = 0;
    int minimum = 0;
    int maximum = 100;
    bool visible = false;
    bool paused = false;
    bool stopped = false;
};
/*!
    Constructs a QWinTaskbarProgress with the parent object \a parent.
 */
QWinTaskbarProgress::QWinTaskbarProgress(QObject *parent) :
    QObject(parent), d_ptr(new QWinTaskbarProgressPrivate)
{
}
/*!
    Destroys the QWinTaskbarProgress.
 */
QWinTaskbarProgress::~QWinTaskbarProgress()
{
}
/*!
    \property QWinTaskbarProgress::value
    \brief the current value of the progress indicator
    The default value is \c 0.
 */
int QWinTaskbarProgress::value() const
{
    Q_D(const QWinTaskbarProgress);
    return d->value;
}
void QWinTaskbarProgress::setValue(int value)
{
    Q_D(QWinTaskbarProgress);
    if ((value == d->value) || value < d->minimum || value > d->maximum)
        return;
    d->value = value;
    emit valueChanged(d->value);
}
/*!
    \property QWinTaskbarProgress::minimum
    \brief the minimum value of the progress indicator
    The default value is \c 0.
 */
int QWinTaskbarProgress::minimum() const
{
    Q_D(const QWinTaskbarProgress);
    return d->minimum;
}
void QWinTaskbarProgress::setMinimum(int minimum)
{
    Q_D(QWinTaskbarProgress);
    setRange(minimum, qMax(minimum, d->maximum));
}
/*!
    \property QWinTaskbarProgress::maximum
    \brief the maximum value of the progress indicator
    The default value is \c 100.
 */
int QWinTaskbarProgress::maximum() const
{
    Q_D(const QWinTaskbarProgress);
    return d->maximum;
}
void QWinTaskbarProgress::setMaximum(int maximum)
{
    Q_D(QWinTaskbarProgress);
    setRange(qMin(d->minimum, maximum), maximum);
}
/*!
    \property QWinTaskbarProgress::visible
    \brief whether the progress indicator is visible.
    The default value is \c false.
 */
bool QWinTaskbarProgress::isVisible() const
{
    Q_D(const QWinTaskbarProgress);
    return d->visible;
}
void QWinTaskbarProgress::setVisible(bool visible)
{
    Q_D(QWinTaskbarProgress);
    if (visible == d->visible)
        return;
    d->visible = visible;
    emit visibilityChanged(d->visible);
}
/*!
    Shows the progress indicator.
 */
void QWinTaskbarProgress::show()
{
    setVisible(true);
}
/*!
    Hides the progress indicator.
 */
void QWinTaskbarProgress::hide()
{
    setVisible(false);
}
/*!
    Sets both the \a minimum and \a maximum values.
 */
void QWinTaskbarProgress::setRange(int minimum, int maximum)
{
    Q_D(QWinTaskbarProgress);
    const bool minChanged = minimum != d->minimum;
    const bool maxChanged = maximum != d->maximum;
    if (minChanged || maxChanged) {
        d->minimum = minimum;
        d->maximum = qMax(minimum, maximum);
        if (d->value < d->minimum || d->value > d->maximum)
            reset();
        if (minChanged)
            emit minimumChanged(d->minimum);
        if (maxChanged)
            emit maximumChanged(d->maximum);
    }
}
/*!
    Resets the progress indicator.
    This function rewinds the value to the minimum value.
 */
void QWinTaskbarProgress::reset()
{
    setValue(minimum());
}
/*!
    \property QWinTaskbarProgress::paused
    \brief whether the progress indicator is paused.
    The default value is \c false.
    The final appearance of a paused progress indicator depends on the active
    Windows theme. Typically, a paused progress indicator turns yellow to indicate
    that the progress is currently paused. Unlike a \l stopped progress indicator,
    a paused progress should not be used to indicate an error.
    \sa pause(), resume()
 */
bool QWinTaskbarProgress::isPaused() const
{
    Q_D(const QWinTaskbarProgress);
    return d->paused;
}
void QWinTaskbarProgress::setPaused(bool paused)
{
    Q_D(QWinTaskbarProgress);
    if (paused == d->paused || d->stopped)
        return;
    d->paused = paused;
    emit pausedChanged(d->paused);
}
/*!
    Pauses the progress indicator.
    \sa paused
 */
void QWinTaskbarProgress::pause()
{
    setPaused(true);
}
/*!
    Resumes a paused or stopped progress indicator.
    \sa paused, stopped
 */
void QWinTaskbarProgress::resume()
{
    Q_D(QWinTaskbarProgress);
    setPaused(false);
    if (d->stopped) {
        d->stopped = false;
        emit stoppedChanged(false);
    }
}
/*!
    \property QWinTaskbarProgress::stopped
    \brief whether the progress indicator is stopped.
    The default value is \c false.
    The final appearance of a stopped progress indicator depends on the active
    Windows theme. Typically, a stopped progress indicator turns red to indicate
    that the progress is currently stopped. Unlike a \l paused progress indicator,
    a stopped progress may be used to indicate an error.
    \sa stop(), resume()
 */
bool QWinTaskbarProgress::isStopped() const
{
    Q_D(const QWinTaskbarProgress);
    return d->stopped;
}
/*!
    Stops the progress indicator.
    \sa stopped
 */
void QWinTaskbarProgress::stop()
{
    Q_D(QWinTaskbarProgress);
    setPaused(false);
    if (!d->stopped) {
        d->stopped = true;
        emit stoppedChanged(true);
    }
}
QT_END_NAMESPACE
/*
 * Copyright (c) 2016 J-P Nurmi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QObject>
#include <QColor>

class StatusBar : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool available READ isAvailable CONSTANT)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(QColor navigationColor READ navigationColor WRITE setNavigationColor)
    Q_PROPERTY(Theme theme READ theme WRITE setTheme)

public:
    explicit StatusBar(QObject *parent = nullptr);

    static bool isAvailable();

    static QColor color();
    static void setColor(const QColor &color);

    static QColor navigationColor();
    static void setNavigationColor(const QColor &color);

    enum Theme { Light, Dark };
    Q_ENUM(Theme)

    static Theme theme();
    static void setTheme(Theme theme);
};

#endif // STATUSBAR_H

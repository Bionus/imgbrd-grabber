/****************************************************************************
** Meta object code from reading C++ file 'aboutwindow.h'
**
** Created: Thu 3. Nov 00:33:41 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../includes/aboutwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'aboutwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_aboutWindow[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,   13,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_aboutWindow[] = {
    "aboutWindow\0\0r\0finished(QNetworkReply*)\0"
};

const QMetaObject aboutWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_aboutWindow,
      qt_meta_data_aboutWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &aboutWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *aboutWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *aboutWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_aboutWindow))
        return static_cast<void*>(const_cast< aboutWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int aboutWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: finished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

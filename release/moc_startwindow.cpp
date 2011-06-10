/****************************************************************************
** Meta object code from reading C++ file 'startwindow.h'
**
** Created: Fri 10. Jun 20:23:30 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../includes/startwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'startwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_startWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x0a,
      20,   12,   12,   12, 0x0a,
      44,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_startWindow[] = {
    "startWindow\0\0save()\0on_openButton_clicked()\0"
    "on_filenameLineEdit_textChanged(QString)\0"
};

const QMetaObject startWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_startWindow,
      qt_meta_data_startWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &startWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *startWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *startWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_startWindow))
        return static_cast<void*>(const_cast< startWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int startWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: save(); break;
        case 1: on_openButton_clicked(); break;
        case 2: on_filenameLineEdit_textChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

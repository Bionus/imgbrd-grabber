/****************************************************************************
** Meta object code from reading C++ file 'filenamewindow.h'
**
** Created: Mon 2. Jan 13:54:39 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../includes/filenamewindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filenamewindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_filenameWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   16,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      45,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_filenameWindow[] = {
    "filenameWindow\0\0,\0validated(QString,QString)\0"
    "accept()\0"
};

void filenameWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        filenameWindow *_t = static_cast<filenameWindow *>(_o);
        switch (_id) {
        case 0: _t->validated((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->accept(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData filenameWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject filenameWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_filenameWindow,
      qt_meta_data_filenameWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &filenameWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *filenameWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *filenameWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_filenameWindow))
        return static_cast<void*>(const_cast< filenameWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int filenameWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void filenameWindow::validated(QString _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'searchwindow.h'
**
** Created: Mon 2. Jan 13:54:56 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../includes/searchwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'searchwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SearchWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      34,   32,   13,   13, 0x0a,
      49,   13,   13,   13, 0x0a,
      58,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SearchWindow[] = {
    "SearchWindow\0\0accepted(QString)\0d\0"
    "setDate(QDate)\0accept()\0"
    "on_buttonImage_clicked()\0"
};

void SearchWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SearchWindow *_t = static_cast<SearchWindow *>(_o);
        switch (_id) {
        case 0: _t->accepted((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->setDate((*reinterpret_cast< QDate(*)>(_a[1]))); break;
        case 2: _t->accept(); break;
        case 3: _t->on_buttonImage_clicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SearchWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SearchWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SearchWindow,
      qt_meta_data_SearchWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SearchWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SearchWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SearchWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SearchWindow))
        return static_cast<void*>(const_cast< SearchWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int SearchWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void SearchWindow::accepted(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'blacklistfix2.h'
**
** Created: Mon 2. Jan 14:18:25 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../includes/blacklistfix2.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'blacklistfix2.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BlacklistFix2[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x08,
      41,   14,   14,   14, 0x08,
      71,   69,   14,   14, 0x08,
      85,   14,   14,   14, 0x28,

       0        // eod
};

static const char qt_meta_stringdata_BlacklistFix2[] = {
    "BlacklistFix2\0\0on_buttonCancel_clicked()\0"
    "on_buttonContinue_clicked()\0p\0"
    "getAll(Page*)\0getAll()\0"
};

void BlacklistFix2::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        BlacklistFix2 *_t = static_cast<BlacklistFix2 *>(_o);
        switch (_id) {
        case 0: _t->on_buttonCancel_clicked(); break;
        case 1: _t->on_buttonContinue_clicked(); break;
        case 2: _t->getAll((*reinterpret_cast< Page*(*)>(_a[1]))); break;
        case 3: _t->getAll(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData BlacklistFix2::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject BlacklistFix2::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_BlacklistFix2,
      qt_meta_data_BlacklistFix2, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BlacklistFix2::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BlacklistFix2::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BlacklistFix2::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BlacklistFix2))
        return static_cast<void*>(const_cast< BlacklistFix2*>(this));
    return QDialog::qt_metacast(_clname);
}

int BlacklistFix2::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'blacklistfix2.h'
**
** Created: Sun 13. Nov 23:55:34 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../includes/blacklistfix2.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'blacklistfix2.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BlacklistFix2[] = {

 // content:
       5,       // revision
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

const QMetaObject BlacklistFix2::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_BlacklistFix2,
      qt_meta_data_BlacklistFix2, 0 }
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
        switch (_id) {
        case 0: on_buttonCancel_clicked(); break;
        case 1: on_buttonContinue_clicked(); break;
        case 2: getAll((*reinterpret_cast< Page*(*)>(_a[1]))); break;
        case 3: getAll(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

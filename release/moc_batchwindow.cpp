/****************************************************************************
** Meta object code from reading C++ file 'batchwindow.h'
**
** Created: Sun 19. Jun 01:02:46 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../includes/batchwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'batchwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_batchWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      22,   12,   12,   12, 0x0a,
      39,   12,   12,   12, 0x0a,
      55,   12,   12,   12, 0x0a,
      69,   12,   12,   12, 0x0a,
      85,   12,   12,   12, 0x0a,
     112,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_batchWindow[] = {
    "batchWindow\0\0closed()\0setText(QString)\0"
    "setLog(QString)\0setValue(int)\0"
    "setMaximum(int)\0on_buttonDetails_clicked()\0"
    "closeEvent(QCloseEvent*)\0"
};

const QMetaObject batchWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_batchWindow,
      qt_meta_data_batchWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &batchWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *batchWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *batchWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_batchWindow))
        return static_cast<void*>(const_cast< batchWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int batchWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: closed(); break;
        case 1: setText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: setLog((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: setValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: setMaximum((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: on_buttonDetails_clicked(); break;
        case 6: closeEvent((*reinterpret_cast< QCloseEvent*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void batchWindow::closed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'optionswindow.h'
**
** Created: Fri 10. Jun 18:44:16 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../includes/optionswindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'optionswindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_optionsWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   15,   14,   14, 0x0a,
      68,   14,   14,   14, 0x0a,
     105,   14,   14,   14, 0x0a,
     131,   14,   14,   14, 0x0a,
     168,   14,   14,   14, 0x0a,
     208,   14,   14,   14, 0x0a,
     248,   14,   14,   14, 0x0a,
     284,   14,   14,   14, 0x0a,
     322,   14,   14,   14, 0x0a,
     357,   14,   14,   14, 0x0a,
     395,   14,   14,   14, 0x0a,
     433,   14,   14,   14, 0x0a,
     467,   14,   14,   14, 0x0a,
     503,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_optionsWindow[] = {
    "optionsWindow\0\0,\0"
    "updateContainer(QTreeWidgetItem*,QTreeWidgetItem*)\0"
    "on_lineFilename_textChanged(QString)\0"
    "on_buttonFolder_clicked()\0"
    "on_lineColoringArtists_textChanged()\0"
    "on_lineColoringCopyrights_textChanged()\0"
    "on_lineColoringCharacters_textChanged()\0"
    "on_lineColoringModels_textChanged()\0"
    "on_lineColoringGenerals_textChanged()\0"
    "on_buttonColoringArtists_clicked()\0"
    "on_buttonColoringCopyrights_clicked()\0"
    "on_buttonColoringCharacters_clicked()\0"
    "on_buttonColoringModels_clicked()\0"
    "on_buttonColoringGenerals_clicked()\0"
    "save()\0"
};

const QMetaObject optionsWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_optionsWindow,
      qt_meta_data_optionsWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &optionsWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *optionsWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *optionsWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_optionsWindow))
        return static_cast<void*>(const_cast< optionsWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int optionsWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateContainer((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 1: on_lineFilename_textChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: on_buttonFolder_clicked(); break;
        case 3: on_lineColoringArtists_textChanged(); break;
        case 4: on_lineColoringCopyrights_textChanged(); break;
        case 5: on_lineColoringCharacters_textChanged(); break;
        case 6: on_lineColoringModels_textChanged(); break;
        case 7: on_lineColoringGenerals_textChanged(); break;
        case 8: on_buttonColoringArtists_clicked(); break;
        case 9: on_buttonColoringCopyrights_clicked(); break;
        case 10: on_buttonColoringCharacters_clicked(); break;
        case 11: on_buttonColoringModels_clicked(); break;
        case 12: on_buttonColoringGenerals_clicked(); break;
        case 13: save(); break;
        default: ;
        }
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

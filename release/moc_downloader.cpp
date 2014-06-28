/****************************************************************************
** Meta object code from reading C++ file 'downloader.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../includes/downloader.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'downloader.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Downloader_t {
    QByteArrayData data[21];
    char stringdata[276];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Downloader_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Downloader_t qt_meta_stringdata_Downloader = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 8),
QT_MOC_LITERAL(2, 20, 0),
QT_MOC_LITERAL(3, 21, 14),
QT_MOC_LITERAL(4, 36, 17),
QT_MOC_LITERAL(5, 54, 12),
QT_MOC_LITERAL(6, 67, 10),
QT_MOC_LITERAL(7, 78, 14),
QT_MOC_LITERAL(8, 93, 13),
QT_MOC_LITERAL(9, 107, 12),
QT_MOC_LITERAL(10, 120, 9),
QT_MOC_LITERAL(11, 130, 3),
QT_MOC_LITERAL(12, 134, 12),
QT_MOC_LITERAL(13, 147, 13),
QT_MOC_LITERAL(14, 161, 16),
QT_MOC_LITERAL(15, 178, 24),
QT_MOC_LITERAL(16, 203, 5),
QT_MOC_LITERAL(17, 209, 4),
QT_MOC_LITERAL(18, 214, 19),
QT_MOC_LITERAL(19, 234, 21),
QT_MOC_LITERAL(20, 256, 19)
    },
    "Downloader\0finished\0\0QNetworkReply*\0"
    "finishedPageCount\0finishedTags\0"
    "QList<Tag>\0finishedImages\0QList<Image*>\0"
    "finishedUrls\0returnInt\0ret\0returnString\0"
    "returnTagList\0returnStringList\0"
    "finishedLoadingPageCount\0Page*\0page\0"
    "finishedLoadingTags\0finishedLoadingImages\0"
    "finishedLoadingUrls"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Downloader[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06 /* Public */,
       4,    1,   82,    2, 0x06 /* Public */,
       5,    1,   85,    2, 0x06 /* Public */,
       7,    1,   88,    2, 0x06 /* Public */,
       9,    1,   91,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,   94,    2, 0x0a /* Public */,
      12,    1,   97,    2, 0x0a /* Public */,
      13,    1,  100,    2, 0x0a /* Public */,
      14,    1,  103,    2, 0x0a /* Public */,
      15,    1,  106,    2, 0x0a /* Public */,
      18,    1,  109,    2, 0x0a /* Public */,
      19,    1,  112,    2, 0x0a /* Public */,
      20,    1,  115,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, 0x80000000 | 6,    2,
    QMetaType::Void, 0x80000000 | 8,    2,
    QMetaType::Void, QMetaType::QStringList,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, 0x80000000 | 6,   11,
    QMetaType::Void, QMetaType::QStringList,   11,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 16,   17,

       0        // eod
};

void Downloader::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Downloader *_t = static_cast<Downloader *>(_o);
        switch (_id) {
        case 0: _t->finished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 1: _t->finishedPageCount((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->finishedTags((*reinterpret_cast< QList<Tag>(*)>(_a[1]))); break;
        case 3: _t->finishedImages((*reinterpret_cast< QList<Image*>(*)>(_a[1]))); break;
        case 4: _t->finishedUrls((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 5: _t->returnInt((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->returnString((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->returnTagList((*reinterpret_cast< QList<Tag>(*)>(_a[1]))); break;
        case 8: _t->returnStringList((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 9: _t->finishedLoadingPageCount((*reinterpret_cast< Page*(*)>(_a[1]))); break;
        case 10: _t->finishedLoadingTags((*reinterpret_cast< Page*(*)>(_a[1]))); break;
        case 11: _t->finishedLoadingImages((*reinterpret_cast< Page*(*)>(_a[1]))); break;
        case 12: _t->finishedLoadingUrls((*reinterpret_cast< Page*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<Image*> >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Page* >(); break;
            }
            break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Page* >(); break;
            }
            break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Page* >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Page* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Downloader::*_t)(QNetworkReply * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Downloader::finished)) {
                *result = 0;
            }
        }
        {
            typedef void (Downloader::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Downloader::finishedPageCount)) {
                *result = 1;
            }
        }
        {
            typedef void (Downloader::*_t)(QList<Tag> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Downloader::finishedTags)) {
                *result = 2;
            }
        }
        {
            typedef void (Downloader::*_t)(QList<Image*> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Downloader::finishedImages)) {
                *result = 3;
            }
        }
        {
            typedef void (Downloader::*_t)(QStringList );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Downloader::finishedUrls)) {
                *result = 4;
            }
        }
    }
}

const QMetaObject Downloader::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Downloader.data,
      qt_meta_data_Downloader,  qt_static_metacall, 0, 0}
};


const QMetaObject *Downloader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Downloader::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Downloader.stringdata))
        return static_cast<void*>(const_cast< Downloader*>(this));
    return QObject::qt_metacast(_clname);
}

int Downloader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void Downloader::finished(QNetworkReply * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Downloader::finishedPageCount(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Downloader::finishedTags(QList<Tag> _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Downloader::finishedImages(QList<Image*> _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Downloader::finishedUrls(QStringList _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'itkbinarymorphopeningfilter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Sources/Filter/itkbinarymorphopeningfilter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'itkbinarymorphopeningfilter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ItkBinaryMorphOpeningFilter_t {
    QByteArrayData data[4];
    char stringdata0[50];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ItkBinaryMorphOpeningFilter_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ItkBinaryMorphOpeningFilter_t qt_meta_stringdata_ItkBinaryMorphOpeningFilter = {
    {
QT_MOC_LITERAL(0, 0, 27), // "ItkBinaryMorphOpeningFilter"
QT_MOC_LITERAL(1, 28, 13), // "radiusChanged"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 6) // "radius"

    },
    "ItkBinaryMorphOpeningFilter\0radiusChanged\0"
    "\0radius"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ItkBinaryMorphOpeningFilter[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       1,   20, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // properties: name, type, flags
       3, QMetaType::Double, 0x00495103,

 // properties: notify_signal_id
       0,

       0        // eod
};

void ItkBinaryMorphOpeningFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ItkBinaryMorphOpeningFilter *_t = static_cast<ItkBinaryMorphOpeningFilter *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->radiusChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ItkBinaryMorphOpeningFilter::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ItkBinaryMorphOpeningFilter::radiusChanged)) {
                *result = 0;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        ItkBinaryMorphOpeningFilter *_t = static_cast<ItkBinaryMorphOpeningFilter *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = _t->getRadius(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        ItkBinaryMorphOpeningFilter *_t = static_cast<ItkBinaryMorphOpeningFilter *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setRadius(*reinterpret_cast< double*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject ItkBinaryMorphOpeningFilter::staticMetaObject = {
    { &Node::staticMetaObject, qt_meta_stringdata_ItkBinaryMorphOpeningFilter.data,
      qt_meta_data_ItkBinaryMorphOpeningFilter,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ItkBinaryMorphOpeningFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ItkBinaryMorphOpeningFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ItkBinaryMorphOpeningFilter.stringdata0))
        return static_cast<void*>(this);
    return Node::qt_metacast(_clname);
}

int ItkBinaryMorphOpeningFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Node::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void ItkBinaryMorphOpeningFilter::radiusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

#ifndef BACKPORTS_QASCONST_H
#define BACKPORTS_QASCONST_H

#include <QtGlobal>


template <typename T>
Q_DECL_CONSTEXPR typename std::add_const<T>::type &qAsConst(T &t) Q_DECL_NOTHROW { return t; }

template <typename T>
void qAsConst(const T &&) Q_DECL_EQ_DELETE;

#endif // BACKPORTS_QASCONST_H

#ifndef SHARED_LIB_GLOBAL_H
#define SHARED_LIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SHARED_LIB_LIBRARY)
#  define SHARED_LIB_EXPORT Q_DECL_EXPORT
#else
#  define SHARED_LIB_EXPORT Q_DECL_IMPORT
#endif

#endif // SHARED_LIB_GLOBAL_H

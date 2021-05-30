#ifndef SHAREDLIB_H
#define SHAREDLIB_H

#include "shared_lib_global.h"
#include <QString>

class SHARED_LIB_EXPORT SharedLib
{
public:
    SharedLib();

    void volwes_to_upper(QString &str)
    {
        QString volwes = "АаЕеЁёИиОоУуыЭэЮюЯяAaEeIiOoUuYy";

        for(QString::iterator it = str.begin(); it<str.end(); ++it)
        {
            if(it->isLetter() && volwes.contains(*it))
                *it = it->toUpper();
        }
    }
};

#endif // SHAREDLIB_H

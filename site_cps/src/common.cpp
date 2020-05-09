#include "common.h"

bool isDirExist(QString fullPath)
{
    QDir dir(fullPath);
    if (dir.exists())
    {
        return true;
    }
    else
    {
        bool ok = dir.mkdir(fullPath);
        return ok;
    }
}

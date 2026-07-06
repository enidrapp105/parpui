#ifndef SQLDATABASE_H
#define SQLDATABASE_H
#include <QMap>
#include "parpui.h"

class SQLDatabase
{
public:
    SQLDatabase();
    void Database_write(Sound* sound);
    QMap<QString, Sound> Database_read();
};

#endif // SQLDATABASE_H

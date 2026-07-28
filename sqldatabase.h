#ifndef SQLDATABASE_H
#define SQLDATABASE_H
#include <QMap>
#include <sqlite3.h>
#include "parpui.h"

class SQLDatabase
{
public:
    SQLDatabase();
    void Database_create();
    int Database_write(Sound* sound);
    QMap<QString, Sound> Database_read();
};

#endif // SQLDATABASE_H

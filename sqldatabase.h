/***********************************
 * Project: PARPUI
 * File:    sqldatabase.h
 * Author:  Enid Rapp
 */
#ifndef SQLDATABASE_H
#define SQLDATABASE_H
#include <QMap>
#include <sqlite3.h>
#include "parpui.h"

class SQLDatabase {
public:
    SQLDatabase();
    void Database_soundinfo_create();
    int Database_soundinfo_update(Sound *sound, SoundENTRY column);
    int Database_soundinfo_write(Sound *sound);
    int Database_soundinfo_write_batch(QMap<QString, Sound>* sounds);
    int Database_soundinfo_remove_row(Sound *sound);
    QMap<QString, Sound> Database_soundinfo_read();
};

#endif // SQLDATABASE_H

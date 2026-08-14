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
    QMap<QString, Sound> Database_soundinfo_read();
    int Database_soundinfo_update(Sound *sound, SoundENTRY column);
    int Database_soundinfo_write(Sound *sound);
    int Database_soundinfo_write_batch(QMap<QString, Sound>* sounds); //debug functon unused
    int Database_soundinfo_remove_row(Sound *sound);


    void Database_appsettings_create();
    AppSettings Database_appsettings_read();
    int Database_appsettings_update(AppSettings *settings, SettingENTRY column);

};

#endif // SQLDATABASE_H

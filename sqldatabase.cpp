/***********************************
 * Project: PARPUI
 * File:    sqldatabase.cpp
 * Author:  Enid Rapp
 */
#include "sqldatabase.h"
#include <QDir>
#include <QDebug>
#include <QStandardPaths>

static int create_callback(void *data, int argc, char **argv, char **azColName) {
    return 0;
}

static int read_callback(void *data, int argc, char **argv, char **azColName) {
    QMap<QString, Sound> *database = static_cast<QMap<QString, Sound>*>(data);
    Sound entry;
    for (int i = 0; i < argc; i++) {
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        if(strcmp(azColName[i], "Color") == 0) {
            entry.button_color = QColor(argv[i]);
        } else if(strcmp(azColName[i], "Name") == 0) {
            entry.sound_name = argv[i];
        } else if(strcmp(azColName[i], "Volume") == 0) {
            char *endptr;
            entry.gain = strtof(argv[i], &endptr);
        } else if(strcmp(azColName[i], "Path") == 0) {
            entry.display_path = argv[i];
        } else if(strcmp(azColName[i], "SoundId") == 0) {
            entry.sound_id = atoi(argv[i]);
        }
    }
    database->insert(entry.display_path, entry);
    //printf("END\n");
    return 0;
}

int rundb(char* dbname, char *query, void* userdata, int (*callbackfunc)(void*, int, char **, char**)) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    rc = sqlite3_open(dbname, &db);
    qDebug() << "Opening db, rc=" << rc;
    qDebug() << "CWD:" << QDir::currentPath();
    if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      return(1);
    }
    rc = sqlite3_exec(db, query, callbackfunc, userdata, &zErrMsg);
    if( rc!=SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    sqlite3_close(db);
    return 0;
}

SQLDatabase::SQLDatabase() {

}

void SQLDatabase::Database_create() {
    char query[] = "CREATE TABLE SoundInfo ("
                   "Color	TEXT,"
                   "Name    TEXT,"
                   "Path    TEXT,"
                   "Volume	REAL,"
                   "SoundId INTEGER UNIQUE,"
                   "PRIMARY KEY(SoundId AUTOINCREMENT)"
                   ");";
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/sounds.db";
    QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    rundb(dbPath.toUtf8().data(), query, NULL, create_callback);
}

QMap<QString, Sound> SQLDatabase::Database_read() {
    QMap<QString, Sound> database;
    char query[] = "SELECT * FROM SoundInfo;";
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/sounds.db";
    QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    qDebug() << "Using db path:" << dbPath;
    rundb(dbPath.toUtf8().data(), query, &database, read_callback);
    return database;
}

int SQLDatabase::Database_write(Sound* sound) {
    //INSERT INTO SoundInfo (Color, Name, Path, Volume)
    //VALUES ('red', 'counting-or-not-counting-gang-violence.mp3', '/home/enid/Working/PARPUI/build/Desktop_Qt_6_11_0-Debug/sounds/counting-or-not-counting-gang-violence.mp3', '1.7')
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc;
    char query[] = "INSERT INTO SoundInfo (Color, Name, Path, Volume)"
                    "VALUES (?, ?, ?, ?)";
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/sounds.db";
    QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    rc = sqlite3_open(dbPath.toUtf8().data(), &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    sqlite3_bind_text(stmt, 1, sound->button_color.name().toUtf8().data(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, sound->sound_name.toUtf8().data(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, sound->display_path.toUtf8().data(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, sound->gain);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return 0;
}

int SQLDatabase::Database_update(Sound* sound, SoundENTRY column) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc;
    QString columnName;
    switch (column) {
    case DISPLAY_PATH: columnName = "Path"; break;
    case SOUND_NAME: columnName = "Name"; break;
    case BUTTON_COLOR: columnName = "Color"; break;
    case GAIN: columnName = "Volume"; break;
    default: qDebug() << "db update invalid column";
        break;
    }

    QString query = QString("UPDATE SoundInfo "
                            "SET \"%1\" = ? "
                            "WHERE SoundId = ?").arg(columnName);
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/sounds.db";
    QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    rc = sqlite3_open(dbPath.toUtf8().data(), &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    rc = sqlite3_prepare_v2(db, query.toUtf8().data(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    switch (column) {
    case DISPLAY_PATH:
        sqlite3_bind_text(stmt, 1, sound->display_path.toUtf8().data(), -1, SQLITE_TRANSIENT);
        break;
    case SOUND_NAME:
        sqlite3_bind_text(stmt, 1, sound->sound_name.toUtf8().data(), -1, SQLITE_TRANSIENT);
        break;
    case BUTTON_COLOR:
        sqlite3_bind_text(stmt, 1, sound->button_color.name().toUtf8().data(), -1, SQLITE_TRANSIENT);
        break;
    case GAIN:
        sqlite3_bind_double(stmt, 1, sound->gain);
        break;
    default: qDebug() << "db update invalid column";
        break;
    }
    sqlite3_bind_int(stmt, 2, sound->sound_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return 0;
}

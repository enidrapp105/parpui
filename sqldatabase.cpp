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
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        if(strcmp(azColName[i], "Color")) {
            entry.button_color = QColor(argv[i]);
        } else if(strcmp(azColName[i], "Name")) {
            entry.sound_name = argv[i];
        } else if(strcmp(azColName[i], "Volume")) {
            char *endptr;
            entry.gain = strtof(argv[i], &endptr);
        } else if(strcmp(azColName[i], "Path")) {
            entry.display_path = argv[i];
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

void SQLDatabase::Database_write(Sound* sound) {
    //INSERT INTO SoundInfo (Color, Name, Path, Volume)
    //VALUES ('red', 'counting-or-not-counting-gang-violence.mp3', '/home/enid/Working/PARPUI/build/Desktop_Qt_6_11_0-Debug/sounds/counting-or-not-counting-gang-violence.mp3', '1.7')
    char query[] = "INSERT INTO SoundInfo (Color, Name, Path, Volume)"
                    "(? ? ? ?)";

}


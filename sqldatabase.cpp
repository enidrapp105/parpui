#include "sqldatabase.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int rundb(char* dbname, char *query) {
  sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
  
    rc = sqlite3_open(dbname, &db);
    if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      return(1);
    }
    rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    sqlite3_close(db);
    return 0;
}

SQLDatabase::SQLDatabase() {
    char query[] = "CREATE TABLE SoundInfo ("
                    "Color	TEXT,"
                    "Volume	REAL,"
                    "SoundId INTEGER,"
                    "PRIMARY KEY(SoundId)"
                    ");";
    rundb("sounds.db", query);
}

void SQLDatabase::Database_create() {

}

QMap<QString, Sound> SQLDatabase::Database_read() {
    QMap<QString, Sound> database;
    return database;
}

void SQLDatabase::Database_write(Sound* sound) {

}


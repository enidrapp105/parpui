#include "sqldatabase.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    fprintf(stderr, "IN CALLBACK argc=%d\n", argc);
    fflush(stderr);
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

}

void SQLDatabase::Database_create() {
    char query[] = "CREATE TABLE SoundInfo ("
                   "Color	TEXT,"
                   "Name    TEXT,"
                   "Volume	REAL,"
                   "SoundId INTEGER UNIQUE,"
                   "PRIMARY KEY(SoundId AUTOINCREMENT)"
                   ");";
    rundb("sounds.db", query);
}

QMap<QString, Sound> SQLDatabase::Database_read() {
    QMap<QString, Sound> database;
    char query[] = "SELECT * FROM SoundInfo;";
    rundb("sounds.db", query);
    return database;
}

void SQLDatabase::Database_write(Sound* sound) {

}


#include "sqldatabase.h"

void testdb(){

}
SQLDatabase::SQLDatabase() {
    sqlite3_open();
}

void SQLDatabase::Database_create() {

}

QMap<QString, Sound> SQLDatabase::Database_read() {
    QMap<QString, Sound> database;
    return database;
}

void SQLDatabase::Database_write(Sound* sound) {

}


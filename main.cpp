/***********************************
 * Project: PARPUI
 * File:    main.cpp
 * Author:  Enid Rapp
 */
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDir>
#include <QObject>
#include <QQmlEngine>
#include <QtConcurrent>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QProcess>
#include <QDesktopServices>
#include <QColor>
#include <regex.h>
#include "parp.h"
#include "parpui.h"
#include "sqldatabase.h"
#include "backend.h"

int main(int argc, char *argv[])
{
    PaError err;
    QProcess process;
    SQLDatabase db;


    err = Pa_Initialize();
    checkErr(err);
    PaStream* keepAliveStream;
    Pa_OpenDefaultStream(&keepAliveStream, 0, 2, paFloat32, 44100, 512, nullptr, nullptr);
    Pa_StartStream(keepAliveStream);

    QGuiApplication app(argc, argv);
    qmlRegisterSingletonType<Backend>("PARPUI", 1, 0, "Backend",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
        return new Backend();
        });
    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("PARPUI", "Main");
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString dbPath = appDataPath + "/sounds.db";
    QDir().mkpath(appDataPath);
    if (!QFile::exists(dbPath)){
        db.Database_create();
    }

    Backend* backend = engine.singletonInstance<Backend*>("PARPUI", "Backend");
    if (backend) {
        backend->load_sounds();
        backend->load_unload_devices();
    }


    int result = QCoreApplication::exec();

    QDir tempdir(QCoreApplication::applicationDirPath() + "/sounds/temp/");
    QStringList files = tempdir.entryList(QStringList() << "*.raw", QDir::Files);
    for (const QString &file : files) {
        tempdir.remove(file);
    }

    err = Pa_Terminate();
    checkErr(err);
    return result;
}

#include "main.moc"
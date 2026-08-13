/***********************************
 * Project: PARPUI
 * File:    backend.h
 * Author:  Enid Rapp
 */
#ifndef BACKEND_H
#define BACKEND_H
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


class Backend : public QObject{
    Q_OBJECT
    Q_PROPERTY(QStringList sounds READ sounds NOTIFY soundsChanged)
    Q_PROPERTY(QString virtual_mic_button_text READ virtual_mic_button_text  NOTIFY virtualmicToggle)
    Q_PROPERTY(int colorVersion READ colorVersion NOTIFY colorChanged)
    //Q_PROPERTY(float volume READ volume WRITE volume_setter NOTIFY volumeChanged)
    //Q_PROPERTY(float indivvolume READ indivvolume WRITE indiv_volume_setter NOTIFY indivvolumeChanged)

public:
    Q_INVOKABLE void play(QString file_name);
    Q_INVOKABLE void load_sounds();
    Q_INVOKABLE void add_sound(QString file_path);
    Q_INVOKABLE void load_unload_devices();
    Q_INVOKABLE void remove_sound(QString file_path);
    Q_INVOKABLE void stop_all();
    Q_INVOKABLE void open_sounds_folder();

    Q_INVOKABLE void volume_setter(float volume){
        if(m_volume != volume){
            m_volume = volume;
        }
    }
    Q_INVOKABLE void indiv_volume_setter(float volume, QString file_path) {
        Sound* sound = find_sound(file_path);
        SQLDatabase db;
        if(sound->gain != volume) {
            sound->gain = volume;
            db.Database_soundinfo_update(sound, GAIN);
        }
    }
    Q_INVOKABLE void color_setter(QColor color, QString file_path) {
        Sound* sound = find_sound(file_path);
        SQLDatabase db;
        if(sound->button_color != color){
            sound->button_color = color;
            db.Database_soundinfo_update(sound, BUTTON_COLOR);
            m_color_version++;
            emit colorChanged();
        }
    }
    Q_INVOKABLE float sound_gain(QString file_path) {
        Sound* sound = find_sound(file_path);
        return sound ? sound->gain : 1.0f;
    }
    Q_INVOKABLE QColor color (QString file_name) {
        Sound *sound = find_sound(file_name);
        return sound->button_color;
    }
    int colorVersion() const { return m_color_version; }
    float volume() const {return m_volume; }
    QStringList sounds() const { return m_sounds.keys(); }
    QString virtual_mic_button_text() const {return m_virtual_mic_button_text; }
signals:
    void soundsChanged();
    void colorChanged();
    void virtualmicToggle();

private:
    void register_sound(paTestData* d) {
        QMutexLocker lock(&m_active_mutex);
        m_active_sounds.append(d);
    }
    void unregister_sound(paTestData* d) {
        QMutexLocker lock(&m_active_mutex);
        m_active_sounds.removeOne(d);
    }
    Sound* find_sound(const QString &file_name_key) {
        auto it = m_sounds.find(file_name_key);
        return (it != m_sounds.end()) ? &it.value() : nullptr;
    }
    float m_volume = 1.0f;
    QMap<QString, Sound> m_sounds;
    QString m_virtual_mic_button_text;
    QString m_sounds_path;
    QMutex m_active_mutex;
    QList<paTestData*> m_active_sounds;
    int m_color_version = 0; //color change trigger
    bool m_virtual_mic_loaded = true;
    bool m_initial_startup = true;
};

#endif // BACKEND_H

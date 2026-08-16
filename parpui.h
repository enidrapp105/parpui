/***********************************
 * Project: PARPUI
 * File:    parpui.h
 * Author:  Enid Rapp
 */
#ifndef PARPUI_H
#define PARPUI_H
#include <QColor>
#include <QObject>
struct Sound {
    QString display_path;   // original file — what the UI shows and buttons reference
    QString playback_path;  // .raw in temp — populated after first play, empty until then
    QString sound_name;
    QColor button_color = 0x3C3C3C;
    float gain = 1.0f;
    int sound_id;
    bool is_converted() const { return !playback_path.isEmpty(); }
};

struct AppSettings {
    float volume = 1.0f;
};

enum SoundENTRY {
    DISPLAY_PATH,
    SOUND_NAME,
    BUTTON_COLOR,
    GAIN,
};

enum SettingENTRY {
    VOLUME
};

#endif // PARPUI_H

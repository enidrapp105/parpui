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
    QString display_path;   // original .mp3 — what the UI shows and buttons reference
    QString playback_path;  // .raw in temp — populated after first play, empty until then
    QString sound_name;
    QColor button_color = "#000000";
    float gain = 1.0f;
    int sound_id;
    bool is_converted() const { return !playback_path.isEmpty(); }
};

enum SoundENTRY{
    DISPLAY_PATH,
    PLAY_BACK_PATH,
    SOUND_NAME,
    BUTTON_COLOR,
    GAIN,
    SOUND_ID,
    IS_CONVERTED
};
#endif // PARPUI_H

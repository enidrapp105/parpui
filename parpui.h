#ifndef PARPUI_H
#define PARPUI_H
#include <QColor>
#include <QObject>
struct Sound {
    QString display_path;   // original .mp3 — what the UI shows and buttons reference
    QString playback_path;  // .raw in temp — populated after first play, empty until then
    QColor button_color;
    float gain = 1.0f;
    bool is_converted() const { return !playback_path.isEmpty(); }
};
#endif // PARPUI_H

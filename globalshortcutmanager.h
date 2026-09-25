#ifndef GLOBALSHORTCUTMANAGER_H
#define GLOBALSHORTCUTMANAGER_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QSet>
#include <QHotkey>
#include "keyreleasewatcher.h"

class Backend;

class GlobalShortcutManager : public QObject {
    Q_OBJECT
public:
    explicit GlobalShortcutManager(Backend *backend, QObject *parent = nullptr);
    //~GlobalShortcutManager() override;
private:
    Backend *m_backend;
    QString m_digits;
    QTimer m_commitTimer;
    QTimer m_turboTimer;
    int m_turboIndex = -1;

    QVector<QHotkey*> m_digitKeys;
    QVector<QHotkey*> m_turboKeys;
    QVector<QHotkey*> m_turboReleaseKeys;

    KeyReleaseWatcher *m_releaseWatcher;
    quint32 m_turboDigitCode = 0;
    QSet<quint32> m_shiftNativeCodes;

    void appendDigit(int d);
    void commit();
    void playIndex(int idx);

    void startTurbo(int idx);
    void stopTurbo(int idx);
    void onNativeKeyReleased(quint32 code);
    quint32 nativeCodeForDigit(int digit) const;


};

#endif // GLOBALSHORTCUTMANAGER_H

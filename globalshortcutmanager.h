#ifndef GLOBALSHORTCUTMANAGER_H
#define GLOBALSHORTCUTMANAGER_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QHotkey>

class Backend;

class GlobalShortcutManager : public QObject {
    Q_OBJECT
public:
    explicit GlobalShortcutManager(Backend *backend, QObject *parent = nullptr);

private:
    Backend *m_backend;
    QString m_digits;
    QTimer m_commitTimer;
    QTimer m_turboTimer;
    int m_turboIndex = -1;

    QVector<QHotkey*> m_digitKeys;
    QVector<QHotkey*> m_turboKeys;
    QVector<QHotkey*> m_turboReleaseKeys;

    void appendDigit(int d);
    void commit();
    void playIndex(int idx);
};

#endif // GLOBALSHORTCUTMANAGER_H

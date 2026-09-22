#ifndef KEYRELEASEWATCHER_H
#define KEYRELEASEWATCHER_H

#include <QObject>
#include <QThread>
#include <QSet>

#ifdef Q_OS_LINUX
struct _XDisplay;
typedef struct _XDisplay Display;
typedef unsigned long XRecordContext;
#endif

class KeyReleaseWatcher : public QThread {
    Q_OBJECT
public:
    explicit KeyReleaseWatcher(QObject *parent = nullptr);
    ~KeyReleaseWatcher() override;
    void requestStop();

signals:
    void nativeKeyReleased(quint32 code);

protected:
    void run() override;

private:
#ifdef Q_OS_LINUX
    Display *m_controlDisplay = nullptr;
    XRecordContext m_context = 0;
#endif
};

#endif // KEYRELEASEWATCHER_H

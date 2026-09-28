#include "globalshortcutmanager.h"
#include "backend.h"

#ifdef Q_OS_LINUX
#include <X11/Xlib.h>
#include <X11/keysym.h>
#elif defined(Q_OS_WIN)
#include <windows.h>
#endif

static const Qt::Key kDigitKeys[10] = {
    Qt::Key_0, Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4,
    Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9,
};

GlobalShortcutManager::GlobalShortcutManager(Backend *backend, QObject *parent)
    : QObject(parent), m_backend(backend) {
    m_commitTimer.setInterval(600);
    m_commitTimer.setSingleShot(true);
    connect(&m_commitTimer, &QTimer::timeout, this, &GlobalShortcutManager::commit);
    m_turboTimer.setInterval(250);
    connect(&m_turboTimer, &QTimer::timeout, this, [this]() {
        if (m_turboIndex >= 0) playIndex(m_turboIndex);
    });
#ifdef Q_OS_LINUX
    if(Display *dpy = XOpenDisplay(nullptr)) {
        m_shiftNativeCodes.insert(XKeysymToKeycode(dpy, XK_Shift_L));
        m_shiftNativeCodes.insert(XKeysymToKeycode(dpy, XK_Shift_R));
        XCloseDisplay(dpy);
    }
#elif defined(Q_OS_WIN)
#endif
    m_releaseWatcher = new KeyReleaseWatcher(this);
    connect(m_releaseWatcher, &KeyReleaseWatcher::nativeKeyReleased,
            this, &GlobalShortcutManager::onNativeKeyReleased);
    m_releaseWatcher->start();
    for (int i = 0; i < 10; ++i) {
        auto *k = new QHotkey(QKeySequence(Qt::ALT | kDigitKeys[i]), true, this);
        connect(k, &QHotkey::activated, this, [this, i]() { appendDigit(i); });
        m_digitKeys.push_back(k);

        auto *t = new QHotkey(QKeySequence(Qt::ALT | Qt::SHIFT | kDigitKeys[i]), true, this);
        connect(t, &QHotkey::activated, this, [this, i]() {
            m_turboIndex = i;
            playIndex(i);
            m_turboTimer.start();
        });
        m_turboKeys.push_back(t);
    }
}

GlobalShortcutManager::~GlobalShortcutManager() {
    m_releaseWatcher->requestStop();
    m_releaseWatcher->wait();
}

quint32 GlobalShortcutManager::nativeCodeForDigit(int digit) const {
#ifdef Q_OS_LINUX
    quint32 code = 0;
    if (Display * dpy = XOpenDisplay(nullptr)) {
        code = XKeysymToKeycode(dpy, XK_0 + digit);
        XCloseDisplay(dpy);
    }
    return code;
#elif defined(Q_OS_WIN)
#else
    return 0;
#endif
}

void GlobalShortcutManager::startTurbo(int idx) {
    m_turboIndex = idx;
    m_turboDigitNativeCode = nativeCodeForDigit(idx);
    playIndex(idx);
    m_turboTimer.start();
}

void GlobalShortcutManager::stopTurbo() {
    m_turboTimer.stop();
    m_turboIndex = -1;
    m_turboDigitNativeCode = 0;
}

void GlobalShortcutManager::onNativeKeyReleased(quint32 code) {
    if (!m_turboTimer.isActive())
        return;
    if (code == m_turboDigitNativeCode || m_shiftNativeCodes.contains(code))
        stopTurbo();
}

void GlobalShortcutManager::appendDigit(int d) {
    m_digits += QString::number(d);
    m_commitTimer.start();
}

void GlobalShortcutManager::commit() {
    if (!m_digits.isEmpty()) {
        int idx = m_digits.toInt() - 1;
        playIndex(idx);
    }
    m_digits.clear();
}

void GlobalShortcutManager::playIndex(int idx) {
    if (idx >= 0 && idx < m_backend->sounds().size()) {
        m_backend->play(m_backend->sounds().at(idx));
    }
}



#include "globalshortcutmanager.h"
#include "backend.h"

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

void GlobalShortcutManager::appendDigit(int d) {
    m_digits += QString::number(d);
    m_turboTimer.start();
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



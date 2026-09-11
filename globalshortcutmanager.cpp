#include "globalshortcutmanager.h"
#include "backend.h"

static const Qt::Key kDigitKeys[10] = {
    Qt::Key_0, Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4,
    Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9,
};

GlobalShortcutManager::GlobalShortcutManager(Backend *backend, QObject *parent) {

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



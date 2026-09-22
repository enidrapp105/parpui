#include "keyreleasewatcher.h"

#ifdef Q_OS_LINUX
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/extensions/record.h>


KeyReleaseWatcher::KeyReleaseWatcher(QObject *parent) : QThread(parent) {}

KeyReleaseWatcher::~KeyReleaseWatcher() {
    requestStop();
    wait();
}

void KeyReleaseWatcher::requestStop() {
    if (m_controlDisplay && m_context) {
        XRecordDisableContext(m_controlDisplay, m_context);
        XFlush(m_controlDisplay);
    }
}

namespace {
struct RecordCallbackData { KeyReleaseWatcher *self; };

void recordCallback(XPointer priv, XRecordInterceptData * data) {
    if (data->category == XRecordFromServer) {
        auto *ev = reinterpret_cast<xEvent *>(data->data);
        if (ev->u.u.type == KeyRelease) {
            auto *self = reinterpret_cast<RecordCallbackData *>(priv)->self;
            emit self->nativeKeyReleased(static_cast<quint32>(ev->u.u.detail));
        }
    }
    XRecordFreeData(data);
}
}

void KeyReleaseWatcher::run() {
    m_controlDisplay = XOpenDisplay(nullptr);
    Display *dataDisplay = XOpenDisplay(nullptr);
    if(!m_controlDisplay || !dataDisplay) {
        qWarning("KeyReleaseWatcher: failed to open X11 display");
        return;
    }

    XRecordClientSpec clients = XRecordAllClients;
    XRecordRange *range = XRecordAllocRange();
    range->device_events.first = KeyPress;
    range->device_events.last = KeyRelease;

    m_context = XRecordCreateContext(m_controlDisplay, 0, &clients, 1, &range, 1);
    XFree(range);
    if(!m_context) {
        qWarning("KeyReleaseWatcher: XRecordCreateContext failed");
        XCloseDisplay(m_controlDisplay);
        XCloseDisplay(dataDisplay);
        m_controlDisplay = nullptr;
        return;
    }
    XSync(m_controlDisplay, True);

    RecordCallbackData cbData{this};
    XRecordEnableContext(dataDisplay,
                         m_context,
                         recordCallback,
                         reinterpret_cast<XPointer>(&cbData));
    XRecordFreeContext(m_controlDisplay, m_context);
    m_context = 0;
    XCloseDisplay(dataDisplay);
    XCloseDisplay(m_controlDisplay);
    m_controlDisplay = nullptr;

}

#else
#error "Keyrelease not supported by OS"
#endif
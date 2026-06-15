import QtQuick
import QtQuick.Controls

Menu{
    title: qsTr("&Device")
    MenuItem{
        text: Backend.virtual_mic_button_text
        onTriggered: Backend.load_unload_devices()
    }
}

import QtQuick.Controls

Menu{
    Menu{
        title: qsTr("&File")
        MenuItem{
            text: "&Add"
            onTriggered: fileDialog.open()
        }
        MenuItem{
            text: "View Sounds in File Explorer"
            onTriggered: Backend.open_sounds_folder()
        }
    }
    Menu{
        title: qsTr("&Device")
        MenuItem{
            text: Backend.virtual_mic_button_text
            onTriggered: Backend.load_unload_devices()
        }
    }
    Menu{
        title: qsTr("&Sound")
        MenuItem{
            text: "Stop"
            onTriggered: Backend.stop_all();
        }
    }

    Menu{
        title: qsTr("&Help")
        MenuItem{
            text: "About"
            onTriggered: aboutDialog.open()
        }
    }
}

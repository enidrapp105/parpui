import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


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
        MenuBarItem {
            contentItem: RowLayout {
                spacing: 8
                Text {
                    text: qsTr("Volume")
                    color: "#FFFFFF"
                }
                Text{
                    text: (volumeSlider.value * 100).toFixed(0) + "%"
                    color: "#FFFFFF"
                }
                Slider{
                    id: volumeSlider
                    from: 0
                    to: 2
                    stepSize: .01
                    onMoved: Backend.volume_setter(value)
                    Layout.fillWidth: true
                }
            }
        }

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

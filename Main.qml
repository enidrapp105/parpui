import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import PARPUI 1.0


Window {
    width: 640
    height: 480
    color: "#1F1F1F"
    visible: true
    title: "PARPUI"

    ColumnLayout{
        MenuBar{

            Menu{
                title: qsTr("&File")
                MenuItem{
                    text: "&Add"
                    onTriggered: fileDialog.open()
                }
            }
            Menu{
                title: qsTr("&Device")
                MenuItem{
                    text: Backend.virtual_mic_button_text
                    onTriggered: Backend.load_unload_devices()
                }
            }
        }
        RowLayout{
            Repeater {
                model: Backend.sounds
                Button {
                    text: modelData.split("/").pop().replace(".raw", "")
                    onClicked: Backend.play(modelData)
                    ContextMenu.menu: Menu {
                        MenuItem {
                            text: qsTr("Remove")
                            onTriggered: Backend.remove_sound(modelData)
                        }
                    }
                }

            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Select a sound file"
        nameFilters: ["MP3 files (*.mp3)", "Raw audio files (*.raw)", "All files (*)"]
        onAccepted: {
            Backend.add_sound(fileDialog.selectedFile)
        }
    }
}

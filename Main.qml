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
                    onTriggered: fileDialog.open();
                }
            }
            Menu{
                title: qsTr("&Device")
                Action {text: qsTr("Load Virtual Mic")}
                Action {text: qsTr("Unload Virtual Mic")}
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
                        }
                    }
                }

            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Select a sound file"
        nameFilters: ["Raw audio files (*.raw)", "All files (*)"]
        onAccepted: {
            Backend.add_sound(fileDialog.selectedFile)
        }
    }
}

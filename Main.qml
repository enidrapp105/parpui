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
    MouseArea{
    anchors.fill: parent
    ContextMenu.menu: Menu{
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
    }

    ColumnLayout{
        MenuBar{

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
        RowLayout{
            Repeater {
                model: Backend.sounds
                Button {
                    text: modelData.split("/").pop().replace(/\.(raw|mp3)$/, "")
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
    Dialog {
        id: aboutDialog
        title: "About PARPUI"
        anchors.centerIn: parent
        modal: true
        standardButtons: Dialog.Ok
        ColumnLayout{
            Text {
                text: qsTr("PARPUI is a soundboard created by Enid Rapp")
                color: "#FFFFFF"
            }
            Text {
                id: link_Text
                text: '<html>Check out the progress on <a href="https://github.com/enidrapp105/parpui">GitHub</a>!</html>'
                onLinkActivated: Qt.openUrlExternally(link)
                linkColor: "#4060FF"
                color: "#FFFFFF"
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

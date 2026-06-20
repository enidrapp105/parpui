import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import PARPUI 1.0
import PARPUI


Window {
    width: 640
    height: 480
    minimumHeight: 200
    minimumWidth: 200
    color: "#1F1F1F"
    visible: true
    title: "PARPUI"
    MouseArea{
        anchors.fill: parent
        ContextMenu.menu: MenuContextBar {}
    }

    ColumnLayout{
        anchors.fill: parent
        MenuBar{
            MenuFile {}
            MenuDevice {}
            MenuSound {}
            MenuHelp {}

            MenuBarItem{
                enabled: false
                text: "︿"

            }
        }
        Flickable {
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentWidth: width
            contentHeight: soundFlow.height

            Flow {
                id: soundFlow
                width: parent.width
                Repeater {
                    model: Backend.sounds
                    Button {
                        text: modelData.split("/").pop().replace(/\.(raw|mp3)$/, "")
                        onClicked: Backend.play(modelData)
                        ContextMenu.menu: Menu {
                            Menu {
                                title: qsTr("Add Tag")
                                TextField {
                                    id: tagField
                                    placeholderText: "example: vine sound"
                                }
                            }

                            MenuItem {
                                text: qsTr("Remove")
                                onTriggered: Backend.remove_sound(modelData)
                            }
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

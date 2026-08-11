/***********************************
 * Project: PARPUI
 * File:    Main.qml
 * Author:  Enid Rapp
 */
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
        }
        GridView {
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
                        text: modelData.replace(/\.(raw|mp3)$/, "")
                        readonly property color customColor: {
                            Backend.colorVersion;
                            return Backend.color(modelData);
                        }
                        readonly property bool hasCustomColor: customColor.a > 0

                        palette.button: hasCustomColor ? customColor : "#3C3C3C"
                        palette.buttonText: hasCustomColor
                            ? (customColor.hslLightness > 0.5 ? "#1A1A1A" : "#E0E0E0")
                            : "#E0E0E0"

                        onClicked: Backend.play(modelData)
                        ContextMenu.menu: Menu {
                            width: 240
                            Menu {
                                title: qsTr("Add Tag")
                                padding: 1

                                TextField {
                                    id: tagField
                                    placeholderText: "example: vine sound"
                                }
                            }
                            MenuBarItem{
                                width: 239
                                topPadding: 1
                                bottomPadding: 1
                                enabled: true
                                contentItem: VolumeControl {
                                    label: qsTr("Sound Volume")
                                    volume: Backend.sound_gain(modelData)
                                    setter: function(v) { Backend.indiv_volume_setter(v, modelData)}
                                }
                            }
                            MenuItem {
                                text: qsTr("Button Color")
                                onTriggered: {
                                    colorDialog.soundname = modelData
                                    colorDialog.open()
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
    ColorDialog {
        id: colorDialog
        property string soundname: ""
        onAccepted: {
            Backend.color_setter(colorDialog.selectedColor, soundname)
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

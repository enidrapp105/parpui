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

/***********************************
 * Component:  Main Window
 * Purpose: Contains the whole ui
 */
Window {
    width: 640
    height: 480
    minimumHeight: 200
    minimumWidth: 200
    color: "#1F1F1F"
    visible: true
    title: "PARPUI"
    /***********************************
     * Component: Main mouse area
     * Purpose: handles context menu region and any mouse actions
     */
    MouseArea {
        anchors.fill: parent
        ContextMenu.menu: MenuContextBar {}
    }

    QtObject {
        id: shortcutEntry
        property string digits : ""
        property bool active : false
        function reset() {
            digits = ""
            active = false
            digitTimer.stop()
        }
        function appendDigit(d) {
            active = true
            digits += String(d)
            digitTimer.restart()
            shortcutCatcher.forceActiveFocus()
        }
        function commit() {
            if (digits.length > 0) {
                var n = parseInt(digits, 10)
                var idx = n - 1
                if (idx >= 0 && idx < Backend.sounds.length) {
                    Backend.play(Backend.sounds[idx])
                }
            }
            reset()
        }
    }
    Timer {
        id: digitTimer
        interval: 600
        repeat: true
        onTriggered: shortcutEntry.commit()
    }

    Shortcut { sequence: "Alt+0"; onActivated: shortcutEntry.appendDigit(0) }
    Shortcut { sequence: "Alt+1"; onActivated: shortcutEntry.appendDigit(1) }
    Shortcut { sequence: "Alt+2"; onActivated: shortcutEntry.appendDigit(2) }
    Shortcut { sequence: "Alt+3"; onActivated: shortcutEntry.appendDigit(3) }
    Shortcut { sequence: "Alt+4"; onActivated: shortcutEntry.appendDigit(4) }
    Shortcut { sequence: "Alt+5"; onActivated: shortcutEntry.appendDigit(5) }
    Shortcut { sequence: "Alt+6"; onActivated: shortcutEntry.appendDigit(6) }
    Shortcut { sequence: "Alt+7"; onActivated: shortcutEntry.appendDigit(7) }
    Shortcut { sequence: "Alt+8"; onActivated: shortcutEntry.appendDigit(8) }
    Shortcut { sequence: "Alt+9"; onActivated: shortcutEntry.appendDigit(9) }

    Item {
        id: shortcutCatcher
        anchors.fill: parent
        focus: true
        z: -1
        Keys.onPressed: (event) => {
            if (!shortcutEntry.active) return
            var isDigit = event.key >= Qt.Key_0 && event.key <= Qt.Key_9
            if (isDigit) {
                shortcutEntry.appendDigit(event.key - Qt.Key_0)
                event.accepted = true
            }
        }
        Keys.onReturnPressed : if (shortcutEntry.active) { digitTimer.stop(); shortcutEntry.commit() }
        Keys.onEnterPressed : if (shortcutEntry.active) { digitTimer.stop(); shortcutEntry.commit() }
        Keys.onEscapePressed : if (shortcutEntry.active) shortcutEntry.reset()
    }

    /***********************************
     * Component: Main column layout
     * Purpose: Contains the content including the menu bar
     */
    ColumnLayout {
        anchors.fill: parent
        MenuBar{
            MenuFile {}
            MenuDevice {}
            MenuSound {}
            MenuHelp {}
        }

        /***********************************
         * Component: Sound board view
         * Purpose: Contains the the soundboard ui
         */
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
                        id: soundButton
                        required property int index
                        required property string modelData
                        text: modelData.replace(/\.(raw|mp3)$/, "")
                        readonly property color customColor: {
                            Backend.colorVersion;
                            return Backend.color(modelData);
                        }
                        readonly property bool hasCustomColor: customColor.a > 0

                        property bool renaming: false



                        readonly property int shortcutNumber: index + 1

                        palette.button: hasCustomColor ? customColor : "#3C3C3C"
                        palette.buttonText: hasCustomColor
                            ? (customColor.hslLightness > 0.5 ? "#1A1A1A" : "#E0E0E0")
                            : "#E0E0E0"

                        onClicked: if(!renaming) Backend.play(modelData)

                        contentItem: Item {
                            //anchors.fill: parent
                            implicitWidth: soundButton.renaming
                                ? Math.max(label.implicitWidth, 100)
                                : label.implicitWidth
                            implicitHeight: soundButton.renaming
                                ? Math.max(label.implicitHeight, renameField.implicitHeight)
                                : label.implicitHeight

                            Text {
                                id: label
                                anchors.fill: parent
                                visible: !soundButton.renaming
                                text: soundButton.text
                                color: soundButton.palette.buttonText
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                elide: Text.ElideRight
                            }

                            TextField {
                                id: renameField
                                anchors.fill: parent
                                visible: soundButton.renaming

                                text: soundButton.text

                                onVisibleChanged: {
                                   if (visible) {
                                        selectAll()
                                        forceActiveFocus()
                                    }
                                }
                                onAccepted: {
                                    soundButton.renaming = false
                                    Backend.name_setter(renameField.text, modelData)
                                }
                                onActiveFocusChanged: {
                                    if (!activeFocus && soundButton.renaming) {
                                        soundButton.renaming = false
                                    }
                                }

                                Keys.onEscapePressed: soundButton.renaming
                            }
                        }

                        ContextMenu.menu: Menu {
                            width: 240
                            MenuItem {
                                text: qsTr("&Rename...")
                                onTriggered: soundButton.renaming = true;
                            }
                            MenuBarItem {
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
    /***********************************
     * Component: about dialog
     * Purpose: contains the about content
     */
    Dialog {
        id: aboutDialog
        title: "About PARPUI"
        anchors.centerIn: parent
        modal: true
        standardButtons: Dialog.Ok
        ColumnLayout {
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
    /***********************************
     * Component: color dialog
     * Purpose: returns the color chosen for buttons
     */
    ColorDialog {
        id: colorDialog
        property string soundname: ""
        onAccepted: {
            Backend.color_setter(colorDialog.selectedColor, soundname)
        }
    }
    /***********************************
     * Component: file add dialog
     * Purpose: returns the real path of the selected file
     */
    FileDialog {
        id: fileDialog
        title: "Select a sound file"
        nameFilters: ["MP3 files (*.mp3)", "Raw audio files (*.raw)", "All files (*)"]
        onAccepted: {
            Backend.add_sound(fileDialog.selectedFile)
        }
    }
}

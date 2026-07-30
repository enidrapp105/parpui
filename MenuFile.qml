/***********************************
 * Project: PARPUI
 * File:    MenuFile.qml
 * Author:  Enid Rapp
 */
import QtQuick
import QtQuick.Controls

Menu{
    title: qsTr("&File")
    MenuItem{
        text: "&Add"
        onTriggered: fileDialog.open()
    }
    MenuItem{
        enabled: false
        text: "Create Folder"
    }

    MenuItem{
        text: "View Sounds in File Explorer"
        onTriggered: Backend.open_sounds_folder()
    }
}

/***********************************
 * Project: PARPUI
 * File:    MenuHelp.qml
 * Author:  Enid Rapp
 */
import QtQuick
import QtQuick.Controls


Menu{
    title: qsTr("&Help")
    MenuItem{
        text: "About"
        onTriggered: aboutDialog.open()
    }
}
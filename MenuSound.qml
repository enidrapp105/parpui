/***********************************
 * Project: PARPUI
 * File:    MenuSound.qml
 * Author:  Enid Rapp
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Menu{
    title: qsTr("&Sound")
    width: 239
    MenuBarItem {
        width: 240
        topPadding: 1
        bottomPadding: 1
        contentItem: VolumeControl{
            id: volumeControl
            label: qsTr("App Volume")
            volume: Backend.volume
            setter: function(v) { Backend.volume_setter(v) }
        }
    }



    MenuItem{
        text: "Stop"
        onTriggered: Backend.stop_all();
    }
}

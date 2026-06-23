import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Menu{
    title: qsTr("&Sound")
    width: 250
    MenuBarItem {
        width: 240
        contentItem: VolumeControl{
            id: volumeControl
            label: qsTr("Master Volume")
            volume: Backend.volume
            setter: function(v) { Backend.volume_setter(v) }
        }
    }



    MenuItem{
        text: "Stop"
        onTriggered: Backend.stop_all();
    }
}

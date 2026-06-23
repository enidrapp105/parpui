import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Menu{
    title: qsTr("&Sound")
    MenuBarItem {
        contentItem: VolumeControl{
            id: volumeControl
            label: qsTr("Volume")
            volume: Backend.volume
            setter: function(v) { Backend.volume_setter(v) }
        }
    }



    MenuItem{
        text: "Stop"
        onTriggered: Backend.stop_all();
    }
}

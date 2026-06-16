import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Menu{
    title: qsTr("&Sound")
    MenuBarItem {
        contentItem: RowLayout {
            spacing: 8
            Text {
                text: qsTr("Volume")
                color: "#FFFFFF"
            }
            TextField{
                text: (Backend.volume * 100).toFixed(0) + "%"
                color: "#FFFFFF"
                implicitWidth: 42
                onEditingFinished: {
                    let val = parseFloat(text.replace("%", " "))
                    if(!isNaN(val)) {
                        val = Math.max(0, Math.min(200, val)) / 100
                        volumeSlider.value = val
                        Backend.volume_setter(val)
                    }
                    text = (volumeSlider.value * 100).toFixed(0) + "%"
                }
            }
            Slider{
                id: volumeSlider
                value: Backend.volume
                from: 0
                to: 2
                stepSize: .01
                onMoved: Backend.volume_setter(value)
                Layout.fillWidth: true
            }
        }
    }



    MenuItem{
        text: "Stop"
        onTriggered: Backend.stop_all();
    }
}

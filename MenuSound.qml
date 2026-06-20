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

                color: "#FFFFFF"
                implicitWidth: 42
                function formatVolume() {
                    return (Backend.volume * 100).toFixed(0) + "%"
                }
                text: formatVolume()

                onEditingFinished: {
                    let val = parseFloat(text.replace("%", " "))
                    if(!isNaN(val)) {
                        val = Math.max(0, Math.min(200, val)) / 100
                        volumeSlider.value = val
                        Backend.volume_setter(val)
                    }
                    text = Qt.binding(formatVolume)
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

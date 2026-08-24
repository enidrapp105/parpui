/***********************************
 * Project: PARPUI
 * File:    VolumeControl.qml
 * Author:  Enid Rapp
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    id: root
    spacing: 8

    property string label: qsTr("Volume")
    property real volume: 1.0
    property real minVolume: 0
    property real maxVolume: 2
    property var setter: function(v) {}
    Text {
        text: root.label
        color: "#fcfbff"
        //Layout.preferredWidth: 90
        elide: Text.ElideRight
    }
    TextField {
        id: textField
        color: "#fcfbff"
        implicitWidth: 42
        function formatVolume() {
            return (root.volume * 100).toFixed(0) + "%"
        }
        text: formatVolume()

        onEditingFinished: {
            let val = parseFloat(text.replace("%", " "))
            if(!isNaN(val)) {
                val = Math.max(root.minVolume * 100, Math.min(root.maxVolume * 100, val)) / 100
                root.volume = val
                root.setter(val)
            }
            text = Qt.binding(formatVolume)
        }
    }
    Slider{
        id: slider
        value: root.volume
        from: root.minVolume
        to: root.maxVolume
        stepSize: .01

        Binding on value {
            value: root.volume
        }

        onMoved: {
            root.volume = value
            root.setter(value)
        }
        Layout.fillWidth: true



    }
    WheelHandler {
        onWheel: (event) => {
            if (event.angleDelta.y > 0) {
                slider.value = Math.min(slider.value + .1, slider.to)
                root.volume = slider.value
                root.setter(slider.value)
            } else {
                slider.value = Math.max(slider.value - .1, slider.from)
                root.volume = slider.value
                root.setter(slider.value)
            }
        }
    }
}

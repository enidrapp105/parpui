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
        color: "#FFFFFF"
        //Layout.preferredWidth: 90
        elide: Text.ElideRight
    }
    TextField {
        id: textField
        color: "#FFFFFF"
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
                slider.value = val
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
        onMoved: root.setter(value)
        Layout.fillWidth: true
        //Layout.preferredWidth: 150
        //Layout.minimumWidth: 100
    }
}

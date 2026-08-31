import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

RowLayout {
    id: root
    property string label: qsTr("Label Unset")
    property bool boundValue: false
    property var setter: function(v) {}
    Text {
        text: root.label
        color: "#fcfbff"
        elide: Text.ElideRight
        Layout.fillWidth: true
    }
    CheckBox {
        //Layout.alignment: Qt.AlignCenter
        onToggled: root.setter(!root.boundValue)
    }
}

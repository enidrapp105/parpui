import QtQuick
import QtQuick.Controls


Menu{
    title: qsTr("&Help")
    MenuItem{
        text: "About"
        onTriggered: aboutDialog.open()
    }
}
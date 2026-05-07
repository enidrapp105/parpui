import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    width: 640
    height: 480
    color: "#1F1F1F"
    visible: true
    title: "PARPUI"
    RowLayout{
        Button{
            text: "Bad to the bone"
            onClicked: Backend.play("/home/enid/Working/PARPUI/build/Desktop_Qt_6_11_0-Debug/_deps/parp-src/bad-to-the-bone-meme.raw")
        }
    }
}

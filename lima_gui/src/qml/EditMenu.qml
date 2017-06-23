import QtQuick 2.0
import QtQuick.Controls 1.4

Row {
    anchors.centerIn: parent
    spacing: parent.width/6

    Button {
        id: cutButton
        text: "Cut"
        width: 80; height: 20
    }
    Button {
        id: copyButton
        text: "Copy"
        width: 80; height: 20
    }
    Button {
        id: pasteButton
        text: "Paste"
        width: 80; height: 20
    }
}

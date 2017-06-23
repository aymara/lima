import QtQuick 2.0
import QtQuick.Controls 1.4

Row {
    anchors.centerIn: parent
    spacing: parent.width/6

    Button {
        id: loadButton
        text: "Load"
        width: 80; height: 20
    }
    Button {
        id: saveButton
        text: "Save"
        width: 80; height: 20
    }
    Button {
        id: exitButton
        text: "Exit"
        width: 80; height: 20

        onClicked: Qt.quit()
    }
}

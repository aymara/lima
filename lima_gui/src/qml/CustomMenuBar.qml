import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQml.Models 2.2
import QtQuick.Controls 1.4 as QtControls

Item {
    width: parent.width
    height: parent.height

    ObjectModel {
        id: menuListModel

        FileMenu {
            width: menuListView.width
            height: menuBar.height
        }

        EditMenu {
            width: menuListView.width
            height: menuBar.height
        }
    }

    ListView {
        id: menuListView

        // Anchors are set to react to window anchors
        anchors.fill: parent
        anchors.bottom: parent.bottom
        width: parent.width
        height: parent.height

        // The model contains the data
        model: menuListModel

        // Control the movement of the menu switching
        snapMode: ListView.SnapOneItem
        orientation: ListView.Horizontal
        boundsBehavior: Flickable.StopAtBounds
        flickDeceleration: 5000
        highlightFollowsCurrentItem: true
        highlightMoveDuration: 240
        highlightRangeMode: ListView.StrictlyEnforceRange
    }

    Rectangle {
        id: textList
        color:"lightblue"
        z: 1
        width: parent.width + 20
        height: 100

        Row {
            anchors.centerIn: parent
            spacing: 1

            Button {
                text: "File"
                id: fileButton

                onClicked: menuListView.currentIndex = 0
            }

            Button {
                id: editButton
                text: "Edit"
                onClicked: menuListView.currentIndex = 1
            }
        }
    }
}

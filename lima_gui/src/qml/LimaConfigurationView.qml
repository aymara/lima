import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import "basics"

Popup {
  id: popup

//   width: parent.width
//   height: parent.height

 width: 800
 height: 600
 x:app_window.width/2 - width/2
 y:app_window.height/2 - height/2

//  SwitchPanel {
//    anchors.fill: parent

//  }

  Rectangle {
    id: content

    anchors.margins: 10
    border { width: 1; color: "gray" }
    anchors.fill: parent

    Column {
      anchors.fill: parent
      anchors.margins: 5

      Row {
        height: parent.height/2
        width: parent.width
        spacing: 2

        Rectangle {
          border { width: 1; color: "lightgray" }
          width: parent.width/2
          height: parent.height

          Rectangle {//Scrollview, but this causes the app to freeze, oddly
            anchors.fill: parent
//            clip: true

//            ScrollBar.vertical.policy: scv.contentHeight > scv.height ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded
//            ScrollBar.horizontal.policy: scv.contentWidth > scv.width ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded

          }

        }

        Rectangle {
          border { width: 1; color: "lightgray" }
          width: parent.width/2 - 2
          height: parent.height
        }

      }

      Rectangle {
        width: parent.width
        height: 2
        color: "transparent"
      }

      Rectangle {
        border { width: 1; color: "lightgray" }
        height: parent.height/2
        width: parent.width
      }
    }

  }
}

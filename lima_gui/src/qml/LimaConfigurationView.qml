import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import "basics"

import integ_cpp 1.0

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

  LimaConfigurationNodeModel {
    id: limaconfig


  }

  ListModel {
    id: lmodel
     ListElement {
        role0: "cat1"
        contents: [
           ListElement {
              name: "aaa"
              someRole1: 123
              checked: 3
           },
          ListElement {
             name: "aaa"
             someRole1: 123
             checked: 3
          },
          ListElement {
             name: "aaa"
             someRole1: 123
             checked: 3
          },
          ListElement {
             name: "aaa"
             someRole1: 123
             checked: 3
          },
          ListElement {
             name: "aaa"
             someRole1: 123
             checked: 3
          },
          ListElement {
             name: "aaa"
             someRole1: 123
             checked: 3
          },
          ListElement {
             name: "aaa"
             someRole1: 123
             checked: 3
          },
          ListElement {
             name: "aaa"
             someRole1: 123
             checked: 3
          },
           ListElement {
              name: "qwAAAer"
              someRole1: 12378
              checked: false
           }
        ]
     }
     ListElement {
        role0: "cat2"
        contents: [
           ListElement {
              name: "bbb"
              someRole1: 123
           },
           ListElement {
              name: "qwBBBer"
              someRole1: 12378
           }
        ]
     }
  }

  Rectangle {
    id: content

    clip: true
    anchors.margins: 10
    border { width: 1; color: "gray" }
    anchors.fill: parent

    TreeView {
      anchors.fill: parent

      model: limaconfig

      TableViewColumn {
        title: "Name"
        role: "name"
        width: 100
      }

      TableViewColumn {
        title: "Active"
        role:"nodeChecked"
        width: 100
      }
    }

  }



}


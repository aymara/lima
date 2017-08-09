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

  LimaConfiguration {
    id: limaconfig


  }

  ListModel {
    id: lmodel
     ListElement {
        role0: "cat1"
        contents: [
           ListElement {
              someRole0: "aaa"
              someRole1: 123
              checkrole: 3
           },
          ListElement {
             someRole0: "aaa"
             someRole1: 123
             checkrole: 3
          },
          ListElement {
             someRole0: "aaa"
             someRole1: 123
             checkrole: 3
          },
          ListElement {
             someRole0: "aaa"
             someRole1: 123
             checkrole: 3
          },
          ListElement {
             someRole0: "aaa"
             someRole1: 123
             checkrole: 3
          },
          ListElement {
             someRole0: "aaa"
             someRole1: 123
             checkrole: 3
          },
          ListElement {
             someRole0: "aaa"
             someRole1: 123
             checkrole: 3
          },
          ListElement {
             someRole0: "aaa"
             someRole1: 123
             checkrole: 3
          },
           ListElement {
              someRole0: "qwAAAer"
              someRole1: 12378
              checkrole: false
           }
        ]
     }
     ListElement {
        role0: "cat2"
        contents: [
           ListElement {
              someRole0: "bbb"
              someRole1: 123
           },
           ListElement {
              someRole0: "qwBBBer"
              someRole1: 12378
           }
        ]
     }
  }

  Rectangle {
    id: content

    anchors.margins: 10
    border { width: 1; color: "gray" }
    anchors.fill: parent

    Column {
      id: lview

      anchors.fill: parent
      anchors.margins: 2

      Repeater {

        model: lmodel

        Rectangle {
          id: categories

          width: lview.width
          height: !hideBtn.checked ? headerect.height + categoryContentRect.height : headerect.height

          border { width: 1; color: "blue"}

          color: "#66DDDDDD"

          Column {

            anchors.fill: parent

            Rectangle {
              id: headerect

              width: parent.width
              height: 30

              color: "#66DDDDDD"

              Row {
                anchors.fill: parent
                Label {
                  text: "Category : " + role0
                  anchors.verticalCenter: parent.verticalCenter

                }

                Button {
                  id: hideBtn

                  anchors.verticalCenter: parent.verticalCenter
                  anchors.right: parent.right
                  anchors.top: parent.top
                  anchors.bottom: parent.bottom

                  checkable: true
                  text: checked ? "+" : "-"
                  width: 30
                  height: 30
                  background: Rectangle {

                    border { width: 1; color: "#AAA" }
                  }
                }

              }
            }

            Rectangle {

              id: categoryContentRect

              visible: !hideBtn.checked

              height: 300

              width: parent.width

              color: "#66DDDDDD"

              border { width: 1; color: "green"}

              Column {

                anchors.fill: parent
                x: 10

                Repeater {
                  model: contents

                  Rectangle {

                    color: "#66DDDDDD"
                    height: 50
                    width: parent.width

                    Text {
                      text: "item : " + someRole0
                      anchors.verticalCenter: parent.verticalCenter
                    }

                    CheckBox {
                      scale: 0.7
                      checked: checkrole
                      anchors.right: parent.right
                      anchors.top: parent.top
                      anchors.bottom: parent.bottom
                    }

                  }
                }

              }
            }
          }

        }
      }

    }
  }



}


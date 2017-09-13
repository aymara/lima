/*
    Copyright 2017 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/
/*!
  @author   Jocelyn Vernay
  @date     Wed, September 06 2017
  */

import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import "basics"


//! This is the previous lima configuration interface. It uses a ListView rather than a TreeView

import integ_cpp 1.0

Popup {
  id: popup

 width: 800
 height: 600
 x:app_window.width/2 - width/2
 y:app_window.height/2 - height/2

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

    Column {
      id: lview

      anchors.fill: parent
      anchors.margins: 2

      Repeater {

        model: limaconfig

        Rectangle {
          id: categoryRect

          clip: true
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
                  text: "Category : (" + id + ") " + name
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

              ScrollView {
                anchors.fill: parent

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
                        text: "item : " + name
                        anchors.verticalCenter: parent.verticalCenter
                      }

                      CheckBox {
                        scale: 0.7
                        checked: nodeChecked
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



}


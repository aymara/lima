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

import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Controls 1.4
// import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2
import "basics"

import integ_cpp 1.0

/*!
    The popup widget for the configuration ui.
    Gives white empty look for now.
  */

Popup {
id: popup

    width: 800
    height: 600
    x:app_window.width/2 - width/2
    y:app_window.height/2 - height/2


//     ConfigurationTreeModel {
//        id: configurationModel
//     }

    StackView {
        id: stack

        anchors.fill: parent

        initialItem: configListView

        Rectangle {
            id: configListView

            anchors.margins: 20

            border { width:1; color:"black"}

            ColumnLayout {

              anchors.fill: parent

//               Text {
//                 text: qsTr("New:")
//               }

              Rectangle {

                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredHeight:350
                border.width: 1
                border.color: "#aaaaaa"
//                 color: "cyan"
                radius: 2


                TreeView {
                  id: lview
                  anchors.fill: parent
//                   model: configurationModel
                  anchors.margins: 2
//                   snapMode: ListView.SnapToItem

                  itemDelegate: Label {
                    text: styleData.value
                    width: parent.width


                    background : Rectangle {
                      id: bgrec

                      color:"transparent"
                    }

                    MouseArea {
                      anchors.fill: parent

                      onEntered: {
                        bgrec.color = "#EEEEEEAA"
                      }

                      onExited: {
                        bgrec.color = "transparent"
                      }

                      onDoubleClicked: {
//                         lview.currentIndex = index
                        finalizeBtn.clicked()
                      }

//                       onClicked: lview.currentIndex = index
                    }

                  }

                  TableViewColumn {
                      title: "name"
                      role: "name"
                  }
//                   highlight: Rectangle { color: "lightblue";  }
                  focus: true

                }
              }

              Rectangle {
                id: button_bar

                Layout.fillWidth: true
                Layout.preferredHeight: 40
//                 color: "pink"
                Row {

                  anchors.centerIn: parent
                  spacing: 2
                  Button {
                    id: finalizeBtn

                    text: qsTr("OK")
                    onClicked: {
                      console.log(lview.currentItem.text)
                      lmodel.handle(lview.currentIndex)
                      close()
                    }
                  }

                  Button {
                    id: cancelBtn

                    text: qsTr("Cancel")
                    onClicked: {
                      close()
                    }
                  }

                }

              }

            }

        }


        Rectangle {
            id: editConfigView
//             color:"yellow"
            anchors.margins: 2
        }

    }

}


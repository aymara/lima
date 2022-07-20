// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
                  model: configurationModel
                  anchors.margins: 2
//                   snapMode: ListView.SnapToItem

                  itemDelegate: Label {
                    text: styleData.value
                    width: parent.width


//                     background : Rectangle {
//                       id: bgrec
// 
//                       color:"transparent"
//                     }

                    MouseArea {
                      anchors.fill: parent

                      onEntered: {
//                         bgrec.color = "#EEEEEEAA"
                      }

                      onExited: {
//                         bgrec.color = "transparent"
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
//                       console.log(lview.currentItem.text)
//                       lmodel.handle(lview.currentIndex)
                      popup.state = "invisible"
                    }
                  }

                  Button {
                    id: cancelBtn

                    text: qsTr("Cancel")
                    onClicked: {
                      popup.state = "invisible"
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


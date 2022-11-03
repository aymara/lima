// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// this can be of use as an example of a popup


// import QtQuick 2.5
// import QtQuick.Window 2.0
// import QtQuick.Controls 2.2
// import QtQuick.Controls.Styles 1.4
// import QtQuick.Layouts 1.3
// 
// import "models"
// 
// /// Popup prompt when you want to open a new 'something'
// /// 'New' in toolbar or 'new' in menu bar (ctrl+n)
// 
// ApplicationWindow {
//   id: createNewElementPopup
//   
// //   background: Rectangle {
// //     radius: 6
// //     border.width: 1
// //     border.color: "gray"
// //   }
// 
//   function open() {
//     visible = true
//   }
//   
//   function close() {
//     visible = false
//   }
// 
//   width: 400
//   height: 400
//   x: app_window.width/2 - this.width/2 + app_window.x
//   y: app_window.height/2 - this.height/2 + app_window.y
//   
//   ColumnLayout {
//   
//     anchors.fill: parent
//     
//     Text {
//       text: qsTr("Nouveau :")
//     }
//     
//     Rectangle {
//       
//       Layout.fillWidth: true
//       Layout.fillHeight: true
//       Layout.preferredHeight:350
//       border.width: 1
//       border.color: "#aaaaaa"
//       radius: 2
// 
//       NewElementModel {
//         id: lmodel
//       }
//       
//       ListView {
//         id: lview
// 
//         anchors.fill: parent
//         anchors.margins: 2
//         snapMode: ListView.SnapToItem
//         model: lmodel
// 
//         delegate: Label {
//           text: name
//           width: parent.width
//           
//           
//           background : Rectangle {
//             id: bgrec
//           
//             color:"transparent"
//           }
//         
//           MouseArea {
//             anchors.fill: parent
//             
//             onEntered: {
//               //bgrec.color = "#EEEEEEAA"
//             }
//             
//             onExited: {
//               //bgrec.color = "#AAAAEEAA"
//             }
//             
//             onDoubleClicked: {
//               lview.currentIndex = index
//               finalizeBtn.clicked()
//             }
//             
//             onClicked: lview.currentIndex = index
//           }
//           
//         }
// 
//         highlight: Rectangle { color: "lightblue";  }
//         focus: true
// 
//       }
//     }
//     
//     Rectangle {
//       id: button_bar
// 
//       Layout.fillWidth: true
//       Layout.preferredHeight: 40
//       
//       Row {
// 
//         anchors.centerIn: parent
//         spacing: 2
//         Button {
//           id: finalizeBtn
// 
//           text: qsTr("Ok")
//           onClicked: {
//             console.log(lview.currentItem.text)
//             lmodel.handle(lview.currentIndex)
//             close()
//           }
//         }
//         
//         Button {
//           id: cancelBtn
// 
//           text: qsTr("Annuler")
//           onClicked: {
//             close()
//           }
//         }
// 
//       }
// 
//     }
// 
//   }
// 
// }
// 

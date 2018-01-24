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
// this can be of use as an example of a popup


// import QtQuick 2.5
// import QtQuick.Window 2.0
// import QtQuick.Controls 2.2
// import QtQuick.Controls.Styles 1.4
// import QtQuick.Layouts 1.2
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

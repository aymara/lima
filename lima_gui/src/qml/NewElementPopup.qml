import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import "models"

/// Popup prompt when you want to open a new 'something'
ApplicationWindow {
  id: createNewElementPopup
  
  width: 400
  height: 400
  x: app_window.width/2 - this.width/2 + app_window.x
  y: app_window.height/2 - this.height/2 + app_window.y
  
//   background: Rectangle {
//     radius: 6
//     border.width: 1
//     border.color: "gray"
//   }

  function open() {
    visible = true
  }
  
  function close() {
    visible = false
  }
  
  ColumnLayout {
  
    anchors.fill: parent
    
    Text {
      text:"Nouveau :"
    }
    
    Rectangle {
      
      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.preferredHeight:350
      border.width: 1
      border.color: "#aaaaaa"
      radius: 2
      
      ListView {
        id: lview
        anchors.fill: parent
        anchors.margins: 2
        snapMode: ListView.SnapToItem
        model: NewElementModel {id: lmodel }
        delegate: Label {
          text: name
          width: parent.width
          
          
          background : Rectangle {
            id: bgrec
          
            color:"transparent"
          }
        
          MouseArea {
            anchors.fill: parent
            
            onEntered: {
              //bgrec.color = "#EEEEEEAA"
            }
            
            onExited: {
              //bgrec.color = "#AAAAEEAA"
            }
            
            onDoubleClicked: {
              lview.currentIndex = index
              finalizeBtn.clicked()
            }
            
            onClicked: lview.currentIndex = index
          }
          
        }
        highlight: Rectangle { color: "lightblue";  }
        focus: true
      }
    }
    
    Rectangle {
      id: button_bar
      Layout.fillWidth: true
      Layout.preferredHeight: 40
      
      Row {
        anchors.centerIn: parent
        spacing: 2
        Button {
          id: finalizeBtn
          text:"Ok"
          onClicked: {
            console.log(lview.currentItem.text)
            lmodel.handle(lview.currentIndex)
            close()
          }
        }
        
        Button {
          id: cancelBtn
          text:"Annuler"
          onClicked: {
            close()
          }
        }
      }
    }
  }
}


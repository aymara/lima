import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.0

/// Unused for now

Window {
  
  visible: true
  width: 640
  height: 480
  
  Rectangle {
    id: bg
    color:"#111111"
    
    anchors.fill: parent
    
  }
  
Rectangle {
  
  anchors.fill: parent
  anchors.margins: 50
  
  TabBar {
    
    property alias color: tabbedview_tabbar_bg.color
    
    id: bar
    width: parent.width
    color: "green"
    
    background: Rectangle {
      id: tabbedview_tabbar_bg
      anchors.fill: parent
      
    }
    
    CustomTabButton {
      text: qsTr("Home")
      color: "purple"
      width: implicitWidth
      height: parent.height
    }
    TabButton {
      text: qsTr("Discover")
      width: implicitWidth
    }
    TabButton {
      text: qsTr("Activity")
      width: implicitWidth
      Rectangle {
        id: bger
        color:"lightblue"
        anchors.fill: parent
      }
      MouseArea {
        onClicked: {
          bger.color = "black"
          parent.clicked()
        }
      }
    }
    TabButton {
      text:"4"
      width: implicitWidth
    }
    TabButton {
      text:"4"
      width: implicitWidth
    }
    TabButton {
      text:"4"
      width: implicitWidth
    }
    TabButton {
      text:"4"
      width: implicitWidth
    }
    TabButton {
      text:"4"
      width: implicitWidth
    }
    TabButton {
      text:"4"
      width: implicitWidth
    }
  }
  
  StackLayout {
    id: slcontainer
    width: parent.width
    height: parent.height - bar.height
    y: bar.height
    currentIndex: bar.currentIndex
    Rectangle {
      
      color: "#633336"
    }
    
    Rectangle {
      color: "#363363"
      
    }
    
    Rectangle {
    color: "#364363"
    }
  }
  
}

}

import QtQuick 2.0

Rectangle {
  
  property color hoverColor: "#ff1111"
  property color regularColor: "#ee0000"
  
  width: 10
  height: 10
  
  color: regularColor
  
  signal clicked()
  
  MouseArea {
    anchors.fill: parent
    
    onEntered : {
      parent.color = parent.hoverColor
    }
    
    onExited: {
      parent.color = parent.regularColor
    }
    
    onClicked: {
      parent.clicked()
    }
  }
  
  Text {
    anchors.centerIn: parent
    text: "x"
  }
  
}

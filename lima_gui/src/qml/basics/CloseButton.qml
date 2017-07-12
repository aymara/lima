import QtQuick 2.0

/// Close button
/// Meant for tabs in tabbedview

Rectangle {
  
  property color hoverColor: "#ff1111"
  property color regularColor: "#444444"
  property color clickColor: "#ee0000"

  signal clicked()
  
  width: 10
  height: 10
  radius: width/2
  color: regularColor
  
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

    onPressed: {
      parent.color = clickColor
    }
  }
  
  Text {
    anchors.centerIn: parent
    text: "x"
  }
  
}

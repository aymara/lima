import QtQuick 2.7

Rectangle {
  anchors.fill: parent
  
  property alias text: txta.text
  
  Rectangle {
    width: animation.width; height: animation.height + 8
    
    AnimatedImage { id: animation; source: "loading2.gif"; anchors.centerIn: parent}
    
//     Rectangle {
//       property int frames: animation.frameCount
//       
//       width: 4; height: 8
//       x: (animation.width - width) * animation.currentFrame / frames
//       y: animation.height
//       color: "red"
//     }
  }
  
  Text {
    anchors.fill: parent
    id: txta
    text: "Waiting ..."
    textFormat: TextEdit.RichText
  }
}

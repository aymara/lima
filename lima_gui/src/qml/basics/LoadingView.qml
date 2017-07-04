import QtQuick 2.7

/// Simple widget with a loading gif to shows the loading

Rectangle {
  width: animation.width; height: animation.height + 8
  anchors.fill: parent
//  color:"#222222";
  
  AnimatedImage {
    id: animation;
    source: "hex-loading.gif";
    anchors.centerIn: parent
    scale: 0.5
  }
  
  //     Rectangle {
  //       property int frames: animation.frameCount
  //       
  //       width: 4; height: 8
  //       x: (animation.width - width) * animation.currentFrame / frames
  //       y: animation.height
  //       color: "red"
  //     }
}

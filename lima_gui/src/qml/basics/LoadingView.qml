/*!
  @author   Jocelyn Vernay
  @date     Wed, September 06 2017
  */

import QtQuick 2.7

//! Simple widget with a loading gif to shows the loading

Rectangle {

  width: animation.width
  height: animation.height + 8
  anchors.fill: parent
  
  AnimatedImage {
    id: animation;

    source: "qrc:qml/resources/hex-loading.gif";
    anchors.centerIn: parent
    scale: 0.5
  }
  
}

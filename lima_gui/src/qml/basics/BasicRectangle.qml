/*!
  @author   Jocelyn Vernay
  @date     Wed, September 06 2017
  */

import QtQuick 2.0
import "../scripts/colors.js" as Colors

//! Simple random colored rect for tests

Rectangle {
  
  color: "#ee00eeaa"
  
  Component.onCompleted: {
    color = Colors.randomColor()
  }
  
  
}



import QtQuick 2.0
import "../scripts/colors.js" as Colors

/// Simple random colored rect for tests

Rectangle {
  
//   function randomChar() {
//     var i = Math.floor((Math.random() * 5) + 0);
//     return String.fromCharCode(97 + i);
//   }
//   
//   function randomColor() {
//     var str = "#"
//     for (var i = 0; i < 6; i++) {
//       str = str + randomChar()
//     }
//     str = str + "aa";
//     //console.log(str);
//     return str;
//   }
  
  color: "#ee00eeaa"
  
  Component.onCompleted: {
    color = Colors.randomColor()
  }
  
  
}

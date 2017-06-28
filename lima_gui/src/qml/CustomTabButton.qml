import QtQuick 2.7
import QtQuick.Controls 2.2
import "basics"

TabButton {
  id: dis_custom_tab_btn
  
  property color selectedColor: "#aaaaaa"
  property color idleColor: "#777777"
  property alias color: this_xyzazeor_bg.color
  property int index: 0
//   property var tabContentItem: 0
  width: implicitWidth
  anchors.bottom: parent.bottom
  
  signal closed()
  
  onClosed: {
    main_tab_bar.closeTab(index)
  }
  
  background : Rectangle {
    id: this_xyzazeor_bg
    color: idleColor
  }
  
  MouseArea {
    anchors.fill: parent
    onClicked: {
      parent.parent.color = randomColor()
      main_tab_bar.select(index)
    }
  }
  
  CloseButton {
    anchors.right: parent.right
    anchors.top: parent.top
    
    onClicked: {
      parent.closed()
    }
  }
  
  function randomChar() {
    var i = Math.floor((Math.random() * 5) + 0);
    return String.fromCharCode(97 + i);
  }
  
  function randomColor() {
    var str = "#"
    for (var i = 0; i < 6; i++) {
      str = str + randomChar()
    }
    str = str + "aa";
    //console.log(str);
    return str;
  }
  
}

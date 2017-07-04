import QtQuick 2.7
import QtQuick.Controls 2.2
import "basics"

/// Goes with CustomTabBar
/// Unused

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
  
}

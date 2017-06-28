import QtQuick 2.0
import QtQuick.Controls 1.4


Text {
  property alias color: lbl_rectbg.color
  property alias border: lbl_rectbg.border
  property alias radius: lbl_rectbg.radius
  property alias gradient: lbl_rectbg.gradient
  
  Rectangle {
    id: lbl_rectbg
    anchors.fill: parent
  }
}

import QtQuick 2.7
import QtQuick.Controls 2.2

/// Simple scrollable widget to display text

Rectangle {
  property alias text: ttext.text
  property alias textFormat: ttext.textFormat
  anchors.fill: parent
  color:"transparent"

  ScrollView {
    id: scv
    anchors.fill: parent
    clip: true
//    contentWidth: ttext.width
//    contentHeight: ttext.height

    ScrollBar.vertical.policy: scv.contentHeight > scv.height ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded
    ScrollBar.horizontal.policy: scv.contentWidth > scv.width ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded

    TextArea {
      id: ttext
      width: implicitWidth
      height: implicitHeight
      selectByMouse: true
      readOnly: true
//      wrapMode: Text.WordWrap

      textFormat: TextEdit.RichText
    }
  }
}

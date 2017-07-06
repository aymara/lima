import QtQuick 2.7
import QtQuick.Controls 2.2

/// Simple scrollable widget to edit text

Rectangle {
  property alias text: ttextarea.text
  property alias textFormat: ttextarea.textFormat
  anchors.fill: parent
  color:"transparent"

  ScrollView {
    id: scv
    anchors.fill: parent
    clip: true
//     contentWidth: ttextarea.width
//     contentHeight: ttextarea.height

    ScrollBar.vertical.policy: scv.contentHeight > scv.height ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded
    ScrollBar.horizontal.policy: scv.contentWidth > scv.width ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded

    TextArea {
      id: ttextarea
      width: implicitWidth
      height: implicitHeight
      selectByMouse: true
//      wrapMode: Text.WordWrap
//      textFormat: TextEdit.RichText

//      property string placeholderText: "Enter text here..."

//      onPressed: {
//        placeholder.visible = false
//      }

//      Text {
//        id:placeholder
//        text: ttextarea.placeholderText
//      }
    }
  }
}


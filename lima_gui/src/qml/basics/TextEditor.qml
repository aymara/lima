import QtQuick 2.7
import QtQuick.Controls 2.2

/// Simple scrollable widget to edit text

Rectangle {


  property alias text: ttextarea.text
  property alias textFormat: ttextarea.textFormat
  property alias wrapMode: ttextarea.wrapMode
  property alias showMenu: menu.visible

  anchors.fill: parent
  color:"transparent"

  Column {

    anchors.topMargin: 10
    anchors.fill: parent

    Rectangle {
      id: menu

      height: 20
      visible: false
      width: parent.width
      anchors.margins: 3
      color: "#eeeeee"

//      Switch {
//        anchors.fill: parent
//        id:wrapswitch
//        scale: 0.7
//        text:"do not wrap"
//        onCheckedChanged:  {
//          console.log("checked=", checked)
//          console.log(ttextarea.wrapMode)
//          ttextarea.wrapMode = checked ? TextEdit.NoWrap : TextEdit.WordWrap
//        }

//      }
    }

    Rectangle {

      width: parent.width
      height: 1
      color: "#dddddd"
      visible: menu.visible
    }

    ScrollView {
      id: scv
      height: parent.height - (menu.visible ? menu.height : 0)
      width: parent.width
      clip: true
      ScrollBar.vertical.policy: scv.contentHeight > scv.height ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded
      ScrollBar.horizontal.policy: scv.contentWidth > scv.width ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded

      TextArea {
        id: ttextarea

        /// this creates a binding loop property error : to fix
        width: implicitWidth
        height: implicitHeight
        selectByMouse: true
//        wrapMode: wrapswitch.checked ? TextEdit.NoWrap : TextEdit.WordWrap
        wrapMode: Text.WordWrap
//        textFormat: TextEdit.RichText

  //      property string placeholderText: "Enter text here..."

  //      onPressed: {
  //        placeholder.visible = false
  //      }

  //      Text {
  //        id:placeholder
  //        text: ttextarea.placeholderText
  //      }
      
      text: "<img src='~/Documents/cea_presentation_location.png'/>"
      //      
        
      }
    }
  }

}


import QtQuick 2.7
import QtQuick.Controls 2.2

/// Simple scrollable widget to display text

Rectangle {

  property alias text: ttext.text
  property alias textFormat: ttext.textFormat
  property alias wrapMode: ttext.wrapMode
  property alias showMenu: menu.visible

  anchors.fill: parent
  color:"transparent"



  Column {

    Rectangle {
      id:menu

      height: 20
      visible: false

//      CheckBox {
//        text:"wrap words"

//        onCheckStateChanged: {

//        }
//      }
    }

    ScrollView {
      id: scv

      width: parent.width
      height: parent.height - (menu.visible ? menu.height : 0)
      clip: true
      ScrollBar.vertical.policy: scv.contentHeight > scv.height ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded
      ScrollBar.horizontal.policy: scv.contentWidth > scv.width ? ScrollBar.AlwaysOn : ScrollBar.AsNeeded

      TextArea {
        id: ttext
        width: implicitWidth
        height: implicitHeight
        selectByMouse: true
        readOnly: true
  //      wrapMode: Text.WordWrap

//        textFormat: TextEdit.RichText
      }
    }

  }
}

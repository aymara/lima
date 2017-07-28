import QtQuick 2.7
import QtQuick.Controls 2.2

/// Simple scrollable widget to display text

Rectangle {

  property alias text: ttext.text
  property alias textFormat: ttext.textFormat
  property alias wrapMode: ttext.wrapMode
  property alias showMenu: menu.visible
  property int maxwidth: 500

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

      anchors.centerIn: parent
      width: Math.max(parent.width, maxwidth)
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

import QtQuick 2.7
import QtQuick.Controls 2.2

Rectangle {
  id: socb

//  property alias name: label.text
  property alias currentIndex: cbbox.currentIndex
  property string name: "Aleau"
  property var model: ['a','b']
  property var keys: []

  signal selected()

  onSelected: {
  }

  function getCurrentItem() {
    return model[currentIndex]
  }

  function getCurrentItemKey() {
//    var i = model.indexOf(getCurrentItem())
//    if (i >= 0) {
//      return keys[i]
//    }
    return keys[currentIndex]
  }

  height: parent.height
  color: "transparent"

  Column {

    anchors.fill: parent
    spacing: 2

    Text {
      id: nameLabel
      y: -5
      visible: text
      text: name
      font.pointSize: 9
      height: 15
    }

    ComboBox {
      id: cbbox

      model: socb.model
      width: parent.width
      height: parent.height - nameLabel.height

      onCurrentIndexChanged: {
        selected()
      }
    }
  }

}

/// original, non-generic version:
/// depending on how the translation is handled, (whether we can also flag list strings as
/// to be translated), we can only use one or the other

//Controls2.ComboBox {
//  id: aOutputType

//  textRole: "text"
//  width: 200

//  function getCurrentItem() {
//    return model.getItem(currentIndex)
//  }

//  model: ListModel {
//    id: cbmodel

//    ListElement {
//      name:"conll"
//      text: "CONLL"
//    }

//    ListElement {
//      name: "named_entities"
//      text: qsTr("Entités nommées")
//    }

//    ListElement {
//      name:"graph"
//      text: ""
//    }
//  }
//}

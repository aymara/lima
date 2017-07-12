import QtQuick 2.7
import QtQuick.Controls 2.2

Rectangle {
  id: socb

//  property alias name: label.text
  property alias currentIndex: cbbox.currentIndex

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

  ComboBox {
    id: cbbox

    anchors.fill: parent
    model: socb.model

    onCurrentIndexChanged: {
      selected()
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

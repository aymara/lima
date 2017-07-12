import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQml.Models 2.2
import "../scripts/DynamicObjectManager.js" as Dom

/// A generic QML element with a list of switchs.
/// TEST FILE. WIP

Rectangle {
  id: switchPanel
  border.width: 2
  function setModel(mdl) {
    Dom.createComponent(mdl, switchPanel)
    if (Dom.success) {
      gridview.model = Dom.obj
    }
  }

  /// get the boolean by switch name
  function getSwitch(name) {

  }

  /// sample model
  ListModel {
    id: lmodel
    ListElement {
      name:"Hello world"
      value: false
    }
    ListElement {
      name:"ConllHandler"
      value: false
    }
    ListElement {
      name:"DotParser"
      value: true
    }
    ListElement {
     name:"XML Raw"
     value: true
    }ListElement {
      name:"XML Raw"
      value: true
     }ListElement {
      name:"XML Raw"
      value: true
     }ListElement {
      name:"XML Raw"
      value: true
     }ListElement {
      name:"XML Raw"
      value: true
     }ListElement {
      name:"XML Raw"
      value: true
     }ListElement {
      name:"XML Raw"
      value: true
     }ListElement {
      name:"XML Raw"
      value: true
     }ListElement {
      name:"XML Raw"
      value: true
     }
  }

  GridView {
    id:gridview
    model: lmodel
    cellWidth: gridview.width/4
    cellHeight: 50
    anchors.fill: parent
    delegate : Switch {
      id: control
      text: name
      checked: value
    }
  }

}

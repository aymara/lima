import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQml.Models 2.2
import QtQuick.Controls.Styles 1.4

import integ_cpp 1.0

Rectangle {
  id:conlltableview

  property alias model: table_view.model

  ConllListModel {
    id: clm
  }
  
  function loadModel(text) {
//    Dom.createComponent("models/ConllListModel.qml",conlltableview)
//    if (Dom.success) {
//      if (model !== null) {
//        model.destroy()
//      }

//      Dom.obj.fromText(text)
//      model = Dom.obj
//    }
//    else {
//      console.log("fts");
//    }
    clm.fromText(text)
    table_view.model = clm
  }
  
  anchors.fill: parent

  // https://github.com/aymara/lima/wiki/LIMA-User-Manual
//  property alias headers: table_view_repeater.model
  property var headers: ["ID", "FORM", "LEMMA", "CPOSTAG", "POSTAG", "NE", "FEATS", "HEAD", "DEPREL", "PHEAD", "PDEPREL"]


  TableView {
    id: table_view

    alternatingRowColors: false

    anchors.fill: parent
    style : TableViewStyle {

    }


//    model: clm

    TableViewColumn { role: "id"; title: "ID"; width: 100 }
    TableViewColumn { role: "form"; title: "FORM"; width: 100 }
    TableViewColumn { role: "lemma"; title: "LEMMA"; width: 100 }
    TableViewColumn { role: "cpostag"; title: "CPOSTAG"; width: 100 }
    TableViewColumn { role: "postag"; title: "POSTAG"; width: 100 }
    TableViewColumn { role: "ne"; title: "NE"; width: 100 }
    TableViewColumn { role: "feats"; title: "FEATS"; width: 100 }
    TableViewColumn { role: "head"; title: "HEAD"; width: 100 }
    TableViewColumn { role: "deprel"; title: "DEPREL"; width: 100 }
    TableViewColumn { role: "phead"; title: "PHEAD"; width: 100 }
    TableViewColumn { role: "pdeprel"; title: "PDEPREL"; width: 100 }

  }
}

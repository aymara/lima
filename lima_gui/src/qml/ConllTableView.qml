import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQml.Models 2.2
import "scripts/DynamicObjectManager.js" as Dom
import "basics"
import QtQuick.Controls.Styles 1.4
import integ_cpp 1.0


/// WIP
/// The widget that will display the CONLL output of an analysis in a table

Rectangle {
  id:conlltableview
  property alias model: table_view.model
  
  function loadModel(text) {
    Dom.createComponent("models/ConllListModel.qml",conlltableview)
    if (Dom.success) {
      if (model !== null) {
        model.destroy()
      }

      Dom.obj.fromText(text)
      model = Dom.obj
    }
    else {
      console.log("fts");
    }
  }
  
  anchors.fill: parent
  objectName: "booyah_tableviewconll"

  TableView {

    style : TableViewStyle {

    }

    id: table_view
    anchors.fill: parent

    model: null

   TableViewColumn {
     role:"id"
     title:"id"
     width: 40
   }

   TableViewColumn {
     role:"form"
     title:"form"
     width: 100
   }

   TableViewColumn {
     role: "lemma"
     title: "lemma"
   }

   TableViewColumn {
     role:"plemma"
     title:"plemma"

   }

   TableViewColumn {
     role:"pos"
     title:"pos"
   }

   TableViewColumn {
     role:"ppos"
     title:"ppos"
   }

   TableViewColumn {
     role:"feat"
     title:"feat"
   }

   TableViewColumn {
     role:"pfeat"
     title:"pfeat"
   }

   TableViewColumn {
     role:"head"
     title:"head"
   }

   TableViewColumn {
     role:"phead"
     title:"phead"
   }

   TableViewColumn {
     role:"deprel"
     title:"deprel"
   }

   TableViewColumn {
     role:"pdeprel"
     title:"pdeprel"
   }
  }
}

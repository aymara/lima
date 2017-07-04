import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQml.Models 2.2
import "scripts/DynamicObjectManager.js" as Dom
import "basics"

/// WIP
/// The widget that will display the CONLL output of an analysis in a table

Rectangle {
  anchors.fill: parent
  objectName: "booyah_tableviewconll"

//   function addElement(id,token,norm,what,entity,undef, target, relationship) {
//     Dom.createComponent("basics/CONLLListElement.qml",tvmodel);
//     if (Dom.success) {
//      Dom.obj.nid = id
//      Dom.obj.token = token
//      Dom.obj.norm_token = norm
//      Dom.obj.token_what = what
//      Dom.obj.named_entity = entity
//      Dom.obj.undefined = undef
//      Dom.obj.target_token = target
//      Dom.obj.relationship = relationship
//       Dom.obj.title = token
//       Dom.obj.author = relationship
//     }
//     else {
//       console.log("couldn't create new element");
//     }
//   }

  ListModel {
    id:tvmodel

  }

  TableView {
    id: table_view
    anchors.fill: parent

   TableViewColumn {
     role:"id"
     title:"id"
     width: 40
   }

   TableViewColumn {
     role:"token"
     title:"Token"
     width: 100
   }

   TableViewColumn {
     role: "norm_token"
     title: "norm_token"
   }

   TableViewColumn {
     role:"token_what"
     title:"what"
   }

   TableViewColumn {
     role:"named_entity"
     title:"Named entity"

   }

   TableViewColumn {
     role:"undefined"
     title:"undefined"
   }

   TableViewColumn {
     role:"target_token"
     title:"target"
   }

   TableViewColumn {
     role:"relationship"
     title:"Relationship"
   }

    model: tvmodel
  }
}

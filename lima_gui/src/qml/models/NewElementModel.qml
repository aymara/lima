import QtQuick 2.7
import QtQml.Models 2.2

ListModel {
  id: lmodel

  function handle(index) {
    switch (index) {
      case 0:
        openAnalyzeTextTab();
        break;
      case 1:
        openAnalyzeFileTab();
        break;
      case 2:
      default:
        break;
    }
  }
  
  ListElement {
    name:"Analyse de texte"
  }
  ListElement {
    name:"Analyse de fichier"
  }
  ListElement {
    name: "Analyse personnalisée"
  }
}

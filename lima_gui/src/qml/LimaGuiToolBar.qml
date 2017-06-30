import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "scripts/colors.js" as Colors

ToolBar {
  
  background: Rectangle {
    color: "#eeeeee"
  }
  
  RowLayout {
    ToolButton {
      text: "New"
      Image {
        width: 30
        height: 30
        //source: "55.png"
      }
      onClicked: {
        createNewElement()
      }
      //iconSource: "new.png"
    }
    ToolButton {
      text:"Open"
      //iconSource: "open.png"
      onClicked: {
        openSelectFileDialog()
      }
    }
    ToolButton {
      text:"Save"
      //iconSource: "save-as.png"
    }
    Item { Layout.fillWidth: true }
    
    ToolButton {
      text: "Analyse Texte"
      
      onClicked: {
        openAnalyzeTextTab()
      }
    }
    
    ToolButton {
      text: "Analyse Fichier"
      
      onClicked: {
        openAnalyzeFileTab()
      }
    }
    
    ToolButton {
      text: "⋮"

      onClicked: {
        textAnalyzer.text = Colors.randomChar()
      }
    }
  }
}

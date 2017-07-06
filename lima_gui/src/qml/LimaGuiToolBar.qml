import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "scripts/colors.js" as Colors

/// ToolBar of the main window

ToolBar {
  
  background: Rectangle {
    color: "#eeeeee"
  }
  
  RowLayout {
    ToolButton {
      text: "Nouvelle analyse"
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
      text:"Ouvrir un fichier"
      //iconSource: "open.png"
      onClicked: {
        openSelectFileDialog()
      }
    }

//    ToolButton {
//      text:"Save"
//      //iconSource: "save-as.png"
//      onClicked: {
//         saveTextFile();
//      }
//    }

    Item { Layout.fillWidth: true }
    ToolSeparator {
      height: parent.height
      width: 1
    }
    
    ToolButton {
      text: "Analyser du texte"
      
      onClicked: {
        openAnalyzeTextTab()
      }
    }
    
    ToolButton {
      text: "Analyser un fichier"
      
      onClicked: {
        openAnalyzeFileTab()
      }
    }

    ToolButton {
      text: "Configuration"

      onClicked: {
        configurationView.open()
      }
    }

    ToolButton {
      text: "Exporter"
      onClicked: {

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

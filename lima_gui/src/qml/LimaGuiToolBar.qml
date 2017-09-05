import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "basics"

/// ToolBar of the main window

ToolBar {
  id: toolBar
  
  background: Rectangle {
    color: "#eeeeee"
  }
  
  RowLayout {

//    ToolButton {

//      text: qsTr("Nouvelle analyse")

//      Image {
//        width: 30
//        height: 30
//        //source: "55.png"
//      }

//      onClicked: {
//        createNewElement()
//      }
//      //iconSource: "new.png"
//    }

    ToolButton {

      text: qsTr("Analyze File")
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

    Item {
      Layout.fillWidth: true
    }

    ToolSeparator {

      height: parent.height
      width: 1
    }
    
    ToolButton {

      text: qsTr("Analyze some text")
      
      onClicked: {
        openAnalyzeTextTab()
      }
    }
    
//    ToolButton {

//      text: qsTr("Analyze a file")
      
//      onClicked: {
//        openAnalyzeFileTab()
//      }
//    }

    ToolButton {

      text: qsTr("Configuration")

      onClicked: {
        configurationView.open()
      }
    }

    ToolButton {

      text: qsTr("About")

      onClicked: {
        aboutMenu.open()
      }
    }
    
    ToolButton {
      id: additionalButton

      text: "⋮"

      onClicked: {
        additionalMenu.x = x
        additionalMenu.open()
      }

    }

  }
}

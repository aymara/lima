/**
 * \author Jocelyn VERNAY
 * \file main.qml
 * \date June 2017
 */

import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3

import integ_cpp 1.0
import "script.js" as Script

/**
 * Main QML File : Basic Text Editor
 */

ApplicationWindow
{
  title:"test-lima-qt"

  id:app_window
  visible: true
  width: 800
  height: 600
  property int pile: 0

  menuBar: MenuBar {
    style: MenuBarStyle {
      background: Rectangle {
        color:"white"
      }
      menuStyle: MenuStyle {
        frame : Rectangle {
          color:"white"
        }
      }
    }

    Menu {
      title: "Fichier"

      MenuItem { text: "Ouvrir un fichier"; }
      MenuItem { text: "Sauvegarder"; }
      MenuItem { text: "Sauvegarder en tant que ..."; }

      MenuItem {
        text: "Quitter"
        onTriggered:  Qt.quit()

      }
    }

    Menu {
      title: "Edition"

      MenuItem { text:"Couper" }
      MenuItem { text:"Copier" }
      MenuItem { text:"Coller" }
    }
    
    Menu {
      title: "Analyse"
      
      MenuItem { text: "Analyse CONLL" }
      MenuItem { text: "Graphe" }
      MenuItem { text: "Entités nommées" }
    }
  }

  toolBar: ToolBar {
    RowLayout {
      ToolButton {
        iconSource: "new.png"
      }
      ToolButton {
        iconSource: "open.png"
      }
      ToolButton {
        iconSource: "save-as.png"
      }
      Item { Layout.fillWidth: true }
    }
  }
  
  
  
  Rectangle {
    id: body
    width: parent.width
    height: parent.height
    
    //////////////////////////////////////////////////
    // Do your bidding here
      
    TextAnalyzer {
      id:textAnalyzer
    }
    
    ColumnLayout {
      
      spacing:2
      anchors.fill: parent
      anchors.margins: 20
      id:layout
    
    SelectFileMenu {
      Layout.fillWidth:true
      Layout.fillHeight:true
      
      
      
      onTriggered: {
        textAnalyzer.filepath = fileUrl
        textAnalyzer.analyzeFile()
      }
      text: "Analyser"
    }
    
    ///////////////////////////////////////////////////
    
    GroupBox {
      id: analyzeTextWidget
      
      Layout.fillWidth:true
      Layout.fillHeight:true
      
      width: 400
      height: 100
      
      title: "Analyser du texte"
      
      TextArea {
        id: text_bunch
      
        width: parent.width - 100
        height: parent.height - 100
        
        anchors.margins: 5
      }
      
      Button {
        text:"Analyze Text"
        onClicked: {
          textAnalyzer.text = text_bunch.text
          textAnalyzer.analyzeText()
        }
        
        anchors.top: text_bunch.bottom
      }
      
      
      
    }
    
    }
    ///////////////////////////////////////////////////
  }
}


/*
About testing :
https://stackoverflow.com/questions/33297489/intention-of-gui-unit-testing-in-qml
http://doc.qt.io/qt-5/qtquick-qtquicktest.html#running-tests
https://stackoverflow.com/questions/4163639/best-approach-to-qt-ui-testing
http://pedromateo.github.io/openhmitester/
www.catedrasaes.org/wiki/OHT

*/

/**
 * \author Jocelyn VERNAY
 * \file main.qml
 * \date June 2017
 */

import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3

import integ_cpp 1.0
import "script.js" as Dom

/**
 * Main QML File : lima_gui
 */

ApplicationWindow
{
  title:"test-lima-qt"

  id:app_window
  visible: true
  width: 1024
  height: 768
  //visibility: Window.Maximized
  property int pile: 0
  
  // ideally, one fonction per feature
  function saveTextFile() {}
  function analyzeConll() {}
  function analyzeGraph() {}
  function openFile() {
    file_manager.chooseFile()
  }
  function saveResults() {}
  function configLima() {}
  function setLimaConfig() {}
  // ...

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

      MenuItem { text: "Ouvrir un fichier";
        onTriggered: {
          openFile()
        }
      }
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
    
    Menu {
      title: "Lima"
      
      Menu {
        title: "Configurations"
        
        // that's where i'd put my preset configs .. if i had one !
        
        MenuItem {
          text: "default"
        }
      }
      
      MenuItem {
        text: "Configurer LIMA"
      }
      
      MenuItem {
        text:"Options"
      }
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
  
  // utilities
  
  Item {
    id: file_manager
    
//     Writer {
//       id: writer_data
//     }
    
    function chooseFile() {
      fm_file_dialog.open()
    }
    
    function saveFile() {
      
    }
    
    function loadFile(urls) {
      for (var i=0;i<urls.length; i++) {
        openFile(urls[i]);
      }
    }
    
    function openFile(url) {
      Dom.createComponent("TabFileTextEditor.qml", main_tab_view)
      Dom.obj.title = url
    }
    
    
    FileDialog {
      id:fm_file_dialog
      
      onAccepted: {
        file_manager.loadFile(fileUrls)
      }
    }
  }
  
  
  
  TextAnalyzer {
    id:textAnalyzer
  }
  
  Rectangle {
    id: body
    anchors.fill: parent
    anchors.margins: 5
    color:"white"
    //////////////////////////////////////////////////
    // Do your bidding here
    
    SplitView {
      
      anchors.fill: parent
      orientation: Qt.Vertical
      
      SplitView {
        
        Layout.fillHeight: true
        Layout.fillWidth: true
        anchors.margins: 5
        orientation: Qt.Horizontal
         
        Rectangle {
          Layout.fillHeight: true
          Layout.preferredWidth: 200
          Layout.minimumWidth: 100
    
            Text {
              text:"filebrowser"
            }
        }
        
        TabView {
          id: main_tab_view
          
          
          Layout.fillHeight: true
          Layout.fillWidth: true
          Layout.minimumWidth: 200
          Layout.preferredHeight: 300
          anchors.margins: 10
          
          Tab {
            title: "analyze file"
            
            SelectFileMenu {
              
              onTriggered: {
                textAnalyzer.filepath = fileUrl
                console.log("tr_analyze")
                textAnalyzer.tr_analyzeFile()
              }
              text: "Analyser"
            }
            
          }
          
          Tab {
            title: "analyze text"
            
            AnalyzeTextWidget {
              
            }
            
          }
          
          Tab {
            title: "file1.txt"
            
            Rectangle {
              
              TextArea {
                font.family: "Hack"
                anchors.fill: parent
                textFormat: TextEdit.RichText
                wrapMode: TextEdit.WordWrap
                
                horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOn
              }
              
            }
          }
        }
        
        Rectangle {
          Layout.fillHeight: true
          Layout.preferredWidth: 200
          Layout.minimumWidth: 100
          anchors.margins: 5
          border.width: 1
          border.color: "gray"
          
          Text {
            text: "properties"
          }
        }
        
      }
      
      Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 200
        Layout.minimumHeight: 100
        anchors.margins: 5
        border.width: 1
        border.color: "gray"
        Text {
          text:"output"
        }
        
      }
      
    }
    
    /*SelectFileMenu {
      
      onTriggered: {
        textAnalyzer.filepath = fileUrl
        console.log("tr_analyze")
        textAnalyzer.tr_analyzeFile()
      }
      text: "Analyser"
    }
    
    ///////////////////////////////////////////////////
    
    AnalyzeTextWidget {
      
    }
    
    }*/
    
    /////////////////////////////////////////////////// END OF BODY
  
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

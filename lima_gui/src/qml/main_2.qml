/**
 * \author Jocelyn VERNAY
 * \file main.qml
 * \date June 2017
 */

import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.2 as Controls2
import QtQuick.Controls 1.4 as Controls1
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3

import integ_cpp 1.0
import "script.js" as Dom

/// Deprecated; kept for reference

Controls1.ApplicationWindow
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
  
  function confirmCloseFile() {
    confirmCloseFileDialog.open()
  }
  
  function closeFile(should_save) {
    if (should_save) {
      
    }
    else {
      
    }
  }
  
  // if some files modifications have not been saved
  function confirmExitApplication() {
    // for every file, check if saved
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
      //https://stackoverflow.com/questions/17647905/adding-tabs-dynamically-in-qml
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
  
  LimaGuiApplication {
    id:textAnalyzer
  }
  
  Controls2.Popup {
    id: confirmCloseFileDialog
    implicitHeight: 100
    x: app_window.width/2 - this.width/2
    y: app_window.height/2 - this.height/2
    
    background: Rectangle {
      radius: 3
      color: "#eeeeee"
    }
    
    ColumnLayout {
      
      Text {
        Layout.fillHeight:true
        
        text: qsTr("Les modifications apportées à ce fichier n'ont pas été enregistrées. Voulez-vous les sauvegarder ?")
        
      }
      
      RowLayout {
        Layout.fillHeight:true
        spacing:5
        Controls1.Button {
          
          
          Layout.fillWidth:true
          
          text: qsTr("Enregistrer")
          onClicked: {
            closeFile(true);
            confirmCloseFileDialog.close()
          }
        }
        
        Controls1.Button {
          
          Layout.fillWidth:true
          
          text: qsTr("Ne pas enregistrer")
          onClicked: {
            closeFile(false);
            confirmCloseFileDialog.close()
          }
        }
        
        Controls1.Button {
          
          Layout.fillWidth:true
          
          text: qsTr("Annuler")
          onClicked: {
            confirmCloseFileDialog.close()
          }
        }
      }
    }
  }
  
  Controls2.Popup {
    id: confirmExitApplicationDialog
  }
  
  /// MENU BAR; TOOL BAR
  
  menuBar: Controls1.MenuBar {
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
    
    Controls1.Menu {
      title: "Fichier"
      
      Controls1.MenuItem { text: "Ouvrir un fichier";
        onTriggered: {
          openFile()
        }
      }
      Controls1.MenuItem { text: "Sauvegarder"; }
      Controls1.MenuItem { text: "Sauvegarder en tant que ..."; }
      
      Controls1.MenuItem {
        text: "Exporter ..."
        onTriggered: confirmCloseFile()
      }
      
      Controls1.MenuItem {
        text: "Quitter"
        onTriggered:  Qt.quit()
        
      }
    }
    
    Controls1.Menu {
      title: "Edition"
      
      Controls1.MenuItem { text:"Couper" }
      Controls1.MenuItem { text:"Copier" }
      Controls1.MenuItem { text:"Coller" }
    }
    
    Controls1.Menu {
      title: "Analyse"
      
      Controls1.MenuItem { text: "Analyse CONLL" }
      Controls1.MenuItem { text: "Graphe" }
      Controls1.MenuItem { text: "Entités nommées" }
    }
    
    Controls1.Menu {
      title: "Lima"
      
      Controls1.Menu {
        title: "Configurations"
        
        // that's where i'd put my preset configs .. if i had one !
        
        Controls1.MenuItem {
          text: "default"
        }
      }
      
      Controls1.MenuItem {
        text: "Configurer LIMA"
      }
      
      Controls1.MenuItem {
        text:"Options"
      }
    }
  }
  
  toolBar: Controls2.ToolBar {
    
    background: Rectangle {
      color: "#eeeeee"
    }
    
    RowLayout {
      Controls1.ToolButton {
        iconSource: "new.png"
      }
      Controls1.ToolButton {
        iconSource: "open.png"
      }
      Controls1.ToolButton {
        iconSource: "save-as.png"
      }
      Item { Layout.fillWidth: true }
      
      Controls1.ToolButton {
        text: "hello"
      }
      
      Controls1.ToolButton {
        text: "⋮"
      }
    }
  }
  
  /// BODY
  
  Rectangle {
    id: body
    anchors.fill: parent
    color:"white"
    //////////////////////////////////////////////////
    // Do your bidding here
    
    Controls1.SplitView {
      
      anchors.fill: parent
      orientation: Qt.Vertical
      
      Controls1.SplitView {
        
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
        
        Controls2.TabBar {
          
          Controls2.TabButton {
            text:"analyze file"
          }
          
          Controls2.TabButton {
            text: "analyze text"
          }
          
          Controls2.TabButton {
            text: "file1.txt"
          }
        
          Controls2.SwipeView {
            id: main_tab_view
            
            
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumWidth: 200
            Layout.preferredHeight: 300
            anchors.margins: 10
            
            Item {
              
              SelectFileMenu {
                
                onTriggered: {
                  
                  textAnalyzer.analyzeFileFromUrl(fileUrl)
                }
                text: "Analyser"
              }
              
            }
            
            Item {
              
              AnalyzeTextWidget {
                
              }
              
            }
            
            Item {
              
              Rectangle {
                color: "transparent"
                Controls1.TextArea {
                  font.family: "Hack"
                  anchors.fill: parent
                  textFormat: TextEdit.RichText
                  wrapMode: TextEdit.WordWrap
                }
                
              }
            }
            
            Item {
              
              TabbedView {
                
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

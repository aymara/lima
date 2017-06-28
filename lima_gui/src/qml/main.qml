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
import "scripts/DynamicObjectManager.js" as Dom
import "scripts/colors.js" as Colors

/**
 * Main QML File : lima_gui
 */

Controls1.ApplicationWindow
{
  title:"test-lima-qt"

  id:app_window
  visible: true
  width: 1024
  height: 768
//   visibility: Window.Maximized
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
  
  menuBar: LimaGuiMenuBar {
    
  }
  
  toolBar: LimaGuiToolBar {
    
  }
  
  /// BODY
  
//   Rectangle {
//     id: body
//     anchors.fill: parent
//     color:"white"
//     //////////////////////////////////////////////////
//     // Do your bidding here
//     
//     Controls1.SplitView {
//       
//       anchors.fill: parent
//       orientation: Qt.Vertical
//       
//       Controls1.SplitView {
//         
//         Layout.fillHeight: true
//         Layout.fillWidth: true
//         anchors.margins: 5
//         orientation: Qt.Horizontal
//          
//         Rectangle {
//           Layout.fillHeight: true
//           Layout.preferredWidth: 200
//           Layout.minimumWidth: 100
//     
//             Text {
//               text:"filebrowser"
//             }
//         }
        
        CustomTabBar {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumWidth: 200
            Layout.preferredHeight: 300
            
            Component.onCompleted: {
              for (var i = 0; i < 5; i++) {
                
                addTab("analyze file " + i, "basics/BasicRectangle.qml");
              }
            }
            
            
            
            
            
        }
        /*
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
      
    }*/
    
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
  
//   }
  
//   Controls1.Button {
//     text:"Click me"
//     onClicked: {
//       Dom.createComponent("TabbedView.qml", app_window)
//     }
//   }
}


/*
About testing :
https://stackoverflow.com/questions/33297489/intention-of-gui-unit-testing-in-qml
http://doc.qt.io/qt-5/qtquick-qtquicktest.html#running-tests
https://stackoverflow.com/questions/4163639/best-approach-to-qt-ui-testing
http://pedromateo.github.io/openhmitester/
www.catedrasaes.org/wiki/OHT

*/

/* About console logs :
 * https://stackoverflow.com/questions/36338000/qml-console-log-and-console-debug-dont-write-to-console
 * */

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
//  visibility: Window.Maximized
  property int pile: 0
  
  // ideally, one fonction per feature
  function saveTextFile() {}
  function openSelectFileDialog() {
    file_manager.chooseFile()
  }
  
  /// ANALYZING
  
  function analyzeText(text) {
    if (textAnalyzer.ready) {
      textAnalyzer.analyzeText(text);
      results_tab_view.addTab("results", "basics/ResultTab.qml");
    }
    else {
      data_tab_view.addTab("analyzer is not ready","FileMenu.qml");
    }
  }
  
  function analyzeFile(filename) {
    if (textAnalyzer.ready) {
      textAnalyzer.analyzeFile(filename);
      results_tab_view.addTab("results", "basics/ResultTab.qml");
    }
    else {
      data_tab_view.addTab("analyzer is not ready","FileMenu.qml");
    }
  }
  
  function analyzeFileFromUrl(url) {
    if (textAnalyzer.ready) {
      textAnalyzer.analyzeFileFromUrl(url);
      results_tab_view.addTab("results", "basics/ResultTab.qml");
    }
    else {
      data_tab_view.addTab("analyzer is not ready","FileMenu.qml");
    }
  }
  
  /// OPENING NEW TABS
  
  /// open a tab to write and analyze text
  function openAnalyzeTextTab() {
    data_tab_view.addTab("Analyze Text", "AnalyzeTextWidget.qml");
  }
  
  /// open a tab to select a file and analyze it
  function openAnalyzeFileTab() {
    data_tab_view.addTab("Analyze File", "AnalyzeFileWidget.qml");
  }
  
  /// CLOSING TABS
  
  function confirmCloseFile() {
    confirmCloseFileDialog.open()
  }
  
  /// 
  function closeFile(should_save) {
    textAnalyzer.closeFile()
  }
  
  // if some files modifications have not been saved
  function confirmExitApplication() {
    // for every file, check if saved
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
  
  /// NEW ELEMENTS
  
  function createNewElement() {
    createNewElementPopup.open()
  }
  
  NewElementPopup {
    id: createNewElementPopup
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
      Dom.createComponent("basics/Tab.qml", data_tab_view)
      Dom.obj.title = url
    }
    
    FileDialog {
      id:fm_file_dialog
      
      onAccepted: {
        file_manager.loadFile(fileUrls)
      }
    }
  }
  
//  LimaGuiApplication {
//    id:textAnalyzer
//  }
  
  Controls2.Popup {
    id: confirmExitApplicationDialog
  }
  
  /// MENU BAR; TOOL BAR
  
  menuBar: LimaGuiMenuBar {
    
  }
  
  toolBar: LimaGuiToolBar {
    
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
      orientation: Qt.Horizontal
      
//       Rectangle {
//         color: "lightgray"
//         Layout.fillHeight: true
//         Layout.minimumWidth:  30
//       }
//       
//       Rectangle {
//         color: "gray"
//         Layout.fillHeight: true
//         Layout.preferredWidth: 1
//       }
      
      Rectangle {
        Layout.fillHeight: true
        Layout.preferredWidth: 300
        Layout.minimumWidth: 100
        
        Text {
          text:"filebrowser"
        }
      }
      
      Controls1.SplitView {
        
        Layout.fillHeight: true
        Layout.fillWidth: true
        anchors.margins: 5
        orientation: Qt.Vertical
        
        Controls1.SplitView {
          
          Layout.fillHeight: true
          Layout.fillWidth: true
          Layout.minimumHeight: 500
          Layout.preferredHeight: 700
          
          TabbedView {
            id: data_tab_view
            
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumWidth: 100
            Layout.preferredWidth: 400
            Layout.preferredHeight: 300

            
          } 
          
          TabbedView {
            id: results_tab_view
            
            
            
          } 
          
        }
        
        Rectangle {
          anchors.margins: 20
          Layout.fillWidth: true
          Layout.fillHeight: true
          Layout.preferredHeight: 300
          Layout.minimumHeight: 100

          Controls1.ScrollView {
            anchors.fill: parent

//            ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
//            ScrollBar.vertical.policy: ScrollBar.AlwaysOn

            Text {
              height: implicitHeight;
              width: implicitWidth;
              text: textAnalyzer.text
              anchors.centerIn: parent
            }
          }
        }
      }
      
      
    }    /*
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
        
    /////////////////////////////////////////////////// END OF BODY
  
  }
  
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

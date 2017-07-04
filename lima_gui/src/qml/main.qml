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

import "scripts/DynamicObjectManager.js" as Dom
import "scripts/colors.js" as Colors

import "basics"

/**
 * Main QML File : lima_gui
 */

Controls1.ApplicationWindow
{
  title:"test-lima-qt"

  id:app_window
  visible: true
  x: 500
  width: 1024
  height: 768
//  visibility: Window.Maximized
  property int pile: 0
  
  // ideally, one fonction per feature
  function saveTextFile() {
    //Dom.obj.addElement("aa","bb","cc","dd","ee","ff","gg","hh","ii")
    //console.log(Dom.obj.objectName)
  }
  function openSelectFileDialog() {
    file_manager.chooseFile()

//    Dom.createComponent("basics/Tab.qml", data_tab_view)
//    Dom.createComponent("CONLLTableView.qml", Dom.obj)
  }
  
  /// ANALYZING
  
  function analyzeText(text) {
    if (textAnalyzer.ready) {
      var wv = workspace.getCurrentWorkView();
      if (wv !== null) {
        wv.setResultView("basics/ResultTab.qml");
        var rt = wv.getResultView();
        textAnalyzer.registerQmlObject("resultView",rt);
        textAnalyzer.analyzeText(text);
      }
      else {
        console.log("There are no works around... How did you manage to call this function ?");
      }
    }
    else {
      console.log("Analyzer is not ready yet!");
    }
  }
  
  function analyzeFile(filename) {
    if (textAnalyzer.ready) {
      var wv = workspace.getCurrentWorkView();
      if (wv !== null) {
        wv.setResultView("basics/ResultTab.qml");
        var rt = wv.getResultView();
        textAnalyzer.registerQmlObject("resultView",rt);
        textAnalyzer.analyzeFile(filename);
      }
      else {
        console.log("There are no works around... How did you manage to call this function ?");
      }
    }
    else {
      console.log("Analyzer is not ready yet!");
    }
  }
  
  function analyzeFileFromUrl(url) {
    if (textAnalyzer.ready) {
      var wv = workspace.getCurrentWorkView();
      if (wv !== null) {
        wv.setResultView("basics/ResultTab.qml");
        var rt = wv.getResultView();
        textAnalyzer.registerQmlObject("resultView",rt);
        textAnalyzer.analyzeFileFromUrl(url);
      }
      else {
        console.log("There are no works around... How did you manage to call this function ?");
      }
    }
    else {
      console.log("Analyzer is not ready yet!");
    }
  }
  
  /// OPENING NEW TABS
  
  /// open a tab to write and analyze text
  function openAnalyzeTextTab() {
    var wv = workspace.addWorkTab("Analyze text", "Text", "AnalyzeTextWidget.qml","");

   }
  
  /// open a tab to select a file and analyze it
  function openAnalyzeFileTab() {
    workspace.addWorkTab("Analyze File", "SelectFile", "AnalyzeFileWidget.qml","");
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
      if (textAnalyzer.openFile(url)) {
//        Dom.createComponent("basics/Tab.qml", data_tab_view)
//        Dom.obj.title = textAnalyzer.fileName
//        Dom.createComponent("basics/TextEditor.qml", Dom.obj);
//        Dom.obj.text = textAnalyzer.fileContent
        var wv = workspace.addWorkTab(textAnalyzer.fileName, "OpenFile", "basics/TextEditor.qml","");
        if (wv !== null) {
          wv.getDataView().text = textAnalyzer.fileContent;
        }
      }

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
  
  menuBar: LimaGuiMenuBar {}
  toolBar: LimaGuiToolBar {}
  
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
          
//          TabbedView {
//            id: data_tab_view
            
//            Layout.fillHeight: true
//            Layout.fillWidth: true
//            Layout.minimumWidth: 100
//            Layout.preferredWidth: 400
//            Layout.preferredHeight: 300
//          }
          
//          TabbedView {
//            id: results_tab_view
//          }

//          WorkView {
//            Component.onCompleted: {
//              setDataView("AnalyzeTextWidget.qml")
//              setResultView("basics/ResultTab.qml")
//            }
//          }

          TabbedWorkspace {
            id: workspace

//            Component.onCompleted: {
//              addWorkTab("hello!","AnalyzeTextWidget.qml","basics/ResultTab.qml");
//            }
          }
          
        }
        
//        Rectangle {
//          anchors.margins: 20
//          Layout.fillWidth: true
//          Layout.fillHeight: true
//          Layout.preferredHeight: 300
//          Layout.minimumHeight: 100

//          Controls1.ScrollView {
//            anchors.fill: parent

////            ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
////            ScrollBar.vertical.policy: ScrollBar.AlwaysOn

//            Text {
//              height: implicitHeight;
//              width: implicitWidth;
//              text: textAnalyzer.text
//              anchors.centerIn: parent
//            }
//          }
//        }

        Rectangle {
          anchors.margins: 20
          Layout.fillWidth: true
          Layout.fillHeight: true
          Layout.preferredHeight: 300
          Layout.minimumHeight: 100

          TextView {
            text: textAnalyzer.text
          }
        }
      }
    }
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

/* About console logs :
 * https://stackoverflow.com/questions/36338000/qml-console-log-and-console-debug-dont-write-to-console
 * */

/*!
  @author   Jocelyn Vernay
  @date     Wed, September 06 2017
  */

import QtQuick 2.7
import QtQuick.Window 2.0

// Some widgets and functionnalities are not available from one package to the other
import QtQuick.Controls 2.2 as Controls2
import QtQuick.Controls 1.4 as Controls1
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3

import "basics"

import "scripts/DynamicObjectManager.js" as Dom

/*!
 * main QML file of lima_gui.
 * Declares the main application window.
 */

Controls1.ApplicationWindow {
  id:app_window
  
  property int pile: 0
  
  //! Open the select file dialog
  function openSelectFileDialog() {
    file_manager.chooseFile()
  }

  /*!   This is the function called for the shortcut to start to analyze.
        From the type of the current workview in focus, it select the right
        analysis function.
  */
  function indiscriminateAnalyze() {
    if (workspace.count()) {
      var wv = workspace.getCurrentWorkView()
      if (wv !== null) {

        textAnalyzer.language = language_selector.getCurrentItemKey()

        switch(wv.type) {
        case "OpenFile":
          analyzeFile(wv.title)
          break;
        case "SelectFile":
          analyzeFileFromUrl(wv.getDataView().fileUrl)
          break;
        case "Text":
          analyzeText(wv.getDataView().text)
          break;
        default:
          console.log("oupsie! ", wv.type)
          break;

        }
      }
      else {

      }
    }
  }
  
  // Analysis Functions
  
  //! Launch an analysis of raw text for the current workview.
  //! It creates and/or makes the resultView visible and sets it back to a loading state.
  function analyzeText(text) {
    if (textAnalyzer.ready) {
      var wv = workspace.getCurrentWorkView();
      if (wv !== null) {
        if (wv.getResultView() === null) {
          wv.setResultView("ResultView.qml");
        }

        wv.getResultView().reset()
        var rt = wv.getResultView();
        rt.formatToShow = format_selector.getCurrentItemKey()
        console.log("formattoshow= ", rt.formatToShow)
        //textAnalyzer.registerQmlObject("resultView",rt);
        textAnalyzer.analyzeText(text, rt);
      }
      else {
        console.log("There are no works around... How did you manage to call this function ?");
      }
    }
    else {
      console.log("Analyzer is not ready yet!");
    }
  }
  
  //! Launch an analysis for a file opened in the application.
  //! It creates and/or makes the resultView visible and sets it back to a loading state.
  function analyzeFile(filename) {
    if (textAnalyzer.ready) {
      var wv = workspace.getCurrentWorkView();
      if (wv !== null) {
        if (wv.getResultView() === null) {
          wv.setResultView("ResultView.qml");
        }
        wv.getResultView().reset()
        var rt = wv.getResultView();
        rt.formatToShow = format_selector.getCurrentItemKey()
        console.log("formattoshow= ", rt.formatToShow)
        //textAnalyzer.registerQmlObject("resultView",rt);
        textAnalyzer.analyzeFile(filename, rt);
      }
      else {
        console.log("There are no works around... How did you manage to call this function ?");
      }
    }
    else {
      console.log("Analyzer is not ready yet!");
    }
  }

  //! Launch an analysis for a file at url.
  //! It creates and/or makes the resultView visible and sets it back to a loading state.
  function analyzeFileFromUrl(url) {
    if (textAnalyzer.ready) {
      var wv = workspace.getCurrentWorkView();
      if (wv !== null) {
        if (wv.getResultView() === null) {
          wv.setResultView("ResultView.qml");
        }
        wv.getResultView().reset()
        var rt = wv.getResultView();
        rt.formatToShow = format_selector.getCurrentItemKey()
        console.log("formattoshow= ", rt.formatToShow)
        //textAnalyzer.registerQmlObject("resultView",rt);
        textAnalyzer.analyzeFileFromUrl(url, rt);
      }
      else {
        console.log("There are no works around... How did you manage to call this function ?");
      }
    }
    else {
      console.log("Analyzer is not ready yet!");
    }
  }
  
  // OPENING NEW TABS
  
  //! Open a tab to write and analyze text.
  function openAnalyzeTextTab() {
    var wv = workspace.addWorkTab("Analyze text", "Text", "basics/TextEditor.qml","");
      console.log("WTF");

   }
  
  //! Open a tab to select a file and analyze it.
  function openAnalyzeFileTab() {
    workspace.addWorkTab("Analyze File", "SelectFile", "AnalyzeFileWidget.qml","");
  }
  
  //! This function was meant to be if there are write/save capabilities for files.
  function confirmCloseFile() {
    confirmCloseFileDialog.open()
  }
  
  //! close the current file.
  function closeFile() {
    textAnalyzer.closeFile()
  }

  //! toggles the configuration view.
  function openConfigurationView() {
    return (configurationView.visible ? configurationView.close() : configurationView.open())
  }

  //! When the comboBoxes of the analysis Bar change values, this is supposed to update
  //! the languageIndex and formatIndex of the current workview in focus.
  //! This isn't functional.
  function updateAnalysisOptions() {
    if (workspace.count()) {
      language_selector.currentIndex = workspace.getCurrentWorkView().languageIndex
      format_selector.currentIndex = workspace.getCurrentWorkView().formatIndex
    }
  }
  
  title: "Lima Gui"
  visible: true
  x: 500
  width: 1024
  height: 768

  // MENU BAR; TOOL BAR

  //! menuBar: LimaGuiMenuBar {}
  toolBar: LimaGuiToolBar { leftPadding: 15}

  //! The configuration view popup is declared directly here. It is invisible by default.
  LimaConfigurationView  {
    id: configurationView
  }

  //! A Controls2.Menu is a Popup. Menu containing additional features like 'Configure Lima Gui ... ', etc.
  Controls2.Menu {
    id: additionalMenu
    y: 2
    background: Rectangle {

      implicitWidth: 200
      color: "#EEEEEE"
      border.color: "#cccccc"
      radius: 4
    }


    Controls2.MenuItem {
      text:qsTr("Configure LIMA Gui")
      onTriggered: {
        
      }
    }
  }
  

  //! An utility Item to handle files inside the application.
  Item {
    id: file_manager
    
    //! Make the select File dialog visible.
    function chooseFile() {
      fm_file_dialog.open()
    }
    
    //! Open multiple files
    function loadFiles(urls) {
      for (var i=0;i<urls.length; i++) {
        openFile(urls[i]);
      }
    }
    
    //! Open a file from url
    function openFile(url) {
      if (textAnalyzer.openFile(url)) {
        var l = url.split(':')
        if (l.length > 1) {
          url = l[1]
        }
        var extension = ""
        l = url.split('.')
        if (l.length) {
          extension = l[l.length-1]
          console.log("extension is: ", extension)
        }


        if (extension === "pdf") {
          var wv = workspace.addWorkTab(textAnalyzer.fileName, "OpenFile", "basics/PdfView.qml","");

          if (wv !== null) {
            wv.getDataView().pathPdf = url
          }
        }
        else {
          var wv = workspace.addWorkTab(textAnalyzer.fileName, "OpenFile", "basics/TextView.qml","");

          if (wv !== null) {
            wv.getDataView().text = textAnalyzer.fileContent;
          }
        }
      }

    }
    
    FileDialog {
      id:fm_file_dialog
      
      onAccepted: {
        file_manager.loadFiles(fileUrls)
      }
    }
  }
  
  // BODY
  
  Rectangle {
    id: body

    anchors.fill: parent
    color:"white"
    anchors.margins: 2
    
    Controls1.SplitView {
      
      anchors.fill: parent
      orientation: Qt.Horizontal

      handleDelegate: Rectangle {
        color : "transparent"
        width: 3
      }
      
      Rectangle {

        Layout.fillHeight: true
        Layout.preferredWidth: 300
        Layout.minimumWidth: 100
        border.width: 1
        border.color: "lightgray"
        anchors.margins: 3
        visible: false
        
        Text {
          text:"filebrowser"
        }


      }
      
      Controls1.SplitView {
        
        Layout.fillHeight: true
        Layout.fillWidth: true
        anchors.margins: 5
        orientation: Qt.Vertical

        handleDelegate: Rectangle {
          color : "transparent"
          height: 3
        }

        Controls1.SplitView {
          
          Layout.fillHeight: true
          Layout.fillWidth: true
          Layout.minimumHeight: 500
          Layout.preferredHeight: 700

          handleDelegate: Rectangle {
            color : "transparent"
            width: 3
          }

          Column {

            anchors.fill: parent
            anchors.centerIn: parent

            //! Main view for the user.
            TabbedWorkspace {
              id: workspace

              height: parent.height - analysisBar.height
              width: parent.width

            }

            //! Analysis bar
            Rectangle {
              id: analysisBar

              height: 60
              width: parent.width
              color:"#aaeeeeee"

              Controls1.SplitView {

                anchors.fill: parent
                orientation: Qt.Horizontal

                handleDelegate: Rectangle {
                  color:"black"
                  height:3
                }

                Rectangle {

                  height: parent.height
                  width: parent.width/2
                  color:"#00eeeeee"

                  Row {
                    id: row

                    width: parent.width
                    height: 40
                    anchors.centerIn: parent
                    spacing: 5

                    SelectOptionComboBox {
                      id: language_selector

                      property var languageDictionnary: {
                          "fre": qsTr("French"),
                          "eng": qsTr("English"),
                          "chi": qsTr("Chinese"),
                          "ger": qsTr("German"),
                          "ara": qsTr("Arabic")
                           // add your languages here
                           // you can list every language here, even if it's not loaded by LIMA
                      }

                      name: "Language"
                      width: 200

                      currentIndex: workspace.count() ? workspace.getCurrentWorkView().languageIndex : 0

                      Component.onCompleted:  {
                        // load supported languages list from textAnalyzer

                          keys = textAnalyzer.languages
                          model = [];
                          for (var i=0;i<keys.length;i++) {
                              model.push(languageDictionnary[keys[i]]);
                          }

                          modelChanged()
                      }

                      onSelected: {
                        if (workspace.count()) {
                          workspace.getCurrentWorkView().languageIndex = currentIndex
                        }
                      }
                    }

                    SelectOptionComboBox {
                      id: format_selector

                      name: "Format"
                      width: 200
                      model: [qsTr("CONLL Format"), qsTr("Named entities"), qsTr("Graph")]
                      keys: ["table","NE","graph"]

                      currentIndex: workspace.count() ? workspace.getCurrentWorkView().formatIndex : 0

                      onSelected: {

                        if (workspace.count()) {
                          workspace.getCurrentWorkView().formatIndex = currentIndex
                        }
                      }
                    }

                    SelectOptionComboBox {
                        id: config_selector

                        name: "Analyzer"
                        width: 200
                        
                        // should be linked to textAnalyzer.configurations (Q_PROPERTY)
                        model: ["Default"].concat(["easy"])
                        keys: ["default", "easy"]
                    }


                    Shortcut {

                      sequence:"Ctrl+Shift+A"
                      onActivated: indiscriminateAnalyze()
                    }

                    Controls2.Button {
                      id:universalAnalysisButton

                      text: qsTr("Analyze")
                      enabled: textAnalyzer.ready
                      onClicked: {
                        indiscriminateAnalyze()
                      }
                    }

                  }
                }

              } // SplitView
            }

          }
          
        }
        
        Rectangle {
          
          id: logView
          
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

        Rectangle {
          Layout.fillWidth: true
          Layout.fillHeight: true
          Layout.preferredHeight: 300
          Layout.minimumHeight: 100
          border.width: 1
          border.color: "lightgray"
          anchors.margins: 3
          visible : false

          ResultView {

          }

          TextEditor {
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

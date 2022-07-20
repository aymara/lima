// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/*!
  @author   Jocelyn Vernay
  @date     Wed, September 06 2017
  */

import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

/*! This is the menu bar of the main application window.
    Unused for now (hidden)
*/
MenuBar {
  
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

    title: qsTr("File")
    
    MenuItem {

      text: qsTr("New")
      shortcut: "Ctrl+N"

      onTriggered: {
        createNewElement()
      }

    }
    
    MenuItem { 

      text: qsTr("Open a file...");
      shortcut: "Ctrl+O"

      onTriggered: {
        openSelectFileDialog()
      }

    }

    MenuItem {
      text: qsTr("Save");
    }

    MenuItem {
      text: qsTr("Save as...");
    }
    
    MenuItem {
      text: qsTr("Export...")
      onTriggered: confirmCloseFile()
    }
    
    MenuItem {
      text: qsTr("Quit")
      onTriggered:  Qt.quit()
    }
  }
  
  Menu {

    title: qsTr("Edit")
    
    MenuItem {
      text:qsTr("Cut")
    }

    MenuItem {
      text:qsTr("Copy")
    }

    MenuItem {
      text:qsTr("Paste")
    }
  }
  
  Menu {

    title: qsTr("Analyze")

    MenuItem {

      text:qsTr("Analyze")
      shortcut:"Ctrl+Shift+A"
      enabled: textAnalyzer.ready ? true : false

      onTriggered: {
        indiscriminateAnalyze();
      }

    }

    MenuItem {

      text: qsTr("Analyze current file")

      enabled: textAnalyzer.ready ? true : false
      onTriggered: {
        if (workspace.count()) {
          var wv = workspace.getCurrentWorkView()
          if (wv !== null) {
            analyzeFile(wv.title)
          }
        }
      }
    }
    
    MenuItem {
      text: qsTr("Analyze some text")
      onTriggered: openAnalyzeTextTab();
    }
    
    MenuItem {
      text: qsTr("Analyze files")
      onTriggered: openAnalyzeFileTab();
    }

    MenuItem {
      text: qsTr("Analyze CONLL")

      onTriggered: {

      }
    }

    MenuItem {
      text: qsTr("Graph")
    }

    MenuItem {
      text: qsTr("Named entities")
    }

    MenuItem {
      text:"Debug Test"
      shortcut:"Ctrl+T"

      onTriggered: {
        textAnalyzer.test()
      }
    }
  }
  
  Menu {

    title: "Lima"
    
    Menu {

      title: qsTr("Configurations")
      
      // that's where i'd put my preset configs .. if i had one !
      
      MenuItem {
        text: "default"
      }
    }
    
    MenuItem {

      text: qsTr("Configure LIMA")

      onTriggered: {
        openConfigurationView()
      }
    }
    
    MenuItem {
      text: qsTr("Options")
    }
  }
}

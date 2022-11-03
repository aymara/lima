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

    title: qsTr("Fichier")
    
    MenuItem {

      text: qsTr("Nouveau ")
      shortcut: "Ctrl+N"

      onTriggered: {
        createNewElement()
      }

    }
    
    MenuItem { 

      text: qsTr("Ouvrir un fichier");
      shortcut: "Ctrl+O"

      onTriggered: {
        openSelectFileDialog()
      }

    }

    MenuItem {
      text: qsTr("Sauvegarder");
    }

    MenuItem {
      text: qsTr("Sauvegarder en tant que ...");
    }
    
    MenuItem {
      text: qsTr("Exporter ...")
      onTriggered: confirmCloseFile()
    }
    
    MenuItem {
      text: qsTr("Quitter")
      onTriggered:  Qt.quit()
    }
  }
  
  Menu {

    title: qsTr("Edition")
    
    MenuItem {
      text:qsTr("Couper")
    }

    MenuItem {
      text:qsTr("Copier")
    }

    MenuItem {
      text:qsTr("Coller")
    }
  }
  
  Menu {

    title: qsTr("Analyse")

    MenuItem {

      text:qsTr("Analyser")
      shortcut:"Ctrl+Shift+A"
      enabled: textAnalyzer.ready ? true : false

      onTriggered: {
        indiscriminateAnalyze();
      }

    }

    MenuItem {

      text: qsTr("Analyser fichier courant")

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
      text: qsTr("Analyser du texte")
      onTriggered: openAnalyzeTextTab();
    }
    
    MenuItem {
      text: qsTr("Analyser des fichiers")
      onTriggered: openAnalyzeFileTab();
    }

    MenuItem {
      text: qsTr("Analyse CONLL")

      onTriggered: {

      }
    }

    MenuItem {
      text: qsTr("Graphe")
    }

    MenuItem {
      text: qsTr("Entités nommées")
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

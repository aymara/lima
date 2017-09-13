/*
    Copyright 2017 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/
/*!
  @author   Jocelyn Vernay
  @date     Wed, September 06 2017
  */

import QtQuick 2.7
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

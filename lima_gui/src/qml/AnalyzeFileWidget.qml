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

import QtQuick 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4

//! Implements a widget that allows to select a file (browse-like) and analyze it

GroupBox {
  id: select_file_menu
  
  property alias fileUrl: file_dialog.fileUrl
  property alias fileName: filename_lbl.text
  property alias text: action_button.text
  
  signal selected()
  signal triggered()
  
  onTriggered: {
    analyzeFileFromUrl(fileUrl)
  }

  title: qsTr("Analyser un fichier")
  
  FileDialog {
    id: file_dialog
    
    onAccepted: {
      filename_lbl.text = fileUrl
      selected()
    }
  }
  
  Row {
    spacing: 20
    
    Button {
      id: browse_button

      text: "Parcourir"
      
      onClicked: {
        file_dialog.open()
      }
    }
  
    Text {
      id: filename_lbl

      text: "..."
      width: 100
      elide: Text.ElideMiddle
      height: browse_button.height
    }
    
    Button {
      id: action_button 

      text: qsTr("Analyser")
      enabled: textAnalyzer.ready ? true : false

      onClicked:  {
        triggered()
      }
    }
    
  }
  
  
  
}

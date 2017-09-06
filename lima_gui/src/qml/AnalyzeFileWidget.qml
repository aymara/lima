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

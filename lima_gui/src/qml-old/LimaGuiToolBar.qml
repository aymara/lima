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

import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Controls.Styles 1.4

import "basics"

//! ToolBar of the main window

ToolBar {
  id: toolBar
  
//   background: Rectangle {
//     color: "#eeeeee"
//   }
  
  RowLayout {

    ToolButton {

      text: qsTr("Analyze File")
      //iconSource: "open.png"
      onClicked: {
        openSelectFileDialog()
      }
    }

    Item {
      Layout.fillWidth: true
    }

//     ToolSeparator {
// 
//       height: parent.height
//       width: 1
//     }
    
    ToolButton {

      text: qsTr("Analyze some text")
      
      onClicked: {
        openAnalyzeTextTab()
      }
    }

//     ToolSeparator {
// 
//       height: parent.height
//       width: 1
//     }
    
    ToolButton {

      text: qsTr("Configuration")

      onClicked: {
        configurationView.open()
      }
    }

    ToolButton {
      id: additionalButton

      text: "⋮"

      onClicked: {
//         additionalMenu.x = x
        additionalMenu.open()
      }

    }

  }
}

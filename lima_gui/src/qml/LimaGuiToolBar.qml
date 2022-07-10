// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/*!
  @author   Jocelyn Vernay
  @date     Wed, September 06 2017
  */

import QtQuick 2.5
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

import "basics"

//! ToolBar of the main window

ToolBar {
  id: toolBar
  
  background: Rectangle {
    color: "#eeeeee"
  }
  
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

    ToolSeparator {

      height: parent.height
      width: 1
    }
    
    ToolButton {

      text: qsTr("Analyze some text")
      
      onClicked: {
        openAnalyzeTextTab()
      }
    }

    ToolSeparator {

      height: parent.height
      width: 1
    }
    
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
        additionalMenu.x = x
        additionalMenu.open()
      }

    }

  }
}

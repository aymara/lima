// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/*!
  @author   Jocelyn Vernay
  @date     Wed, September 06 2017
  */

import QtQuick 2.0
import QtQuick.Controls 1.4

import "basics"

/// Widget allowing to input text and analyze it on the fly

Rectangle {
  id: analyzeTextWidget

  property alias text: textv.text  

  anchors.fill: parent
  
//  title: "Analyser du texte"
  
  Rectangle {
    id: text_bunch

    width: parent.width
    height: parent.height - 50
    
    anchors.margins: 5

    TextEditor {
      id:textv
    }
  }
  
  Button {

    text: qsTr("Analyze Text")

    onClicked: {
      analyzeText(textv.text)
    }

    enabled: textAnalyzer.ready ? true : false
    anchors.top: text_bunch.bottom

    ToolTip.delay: 1000
    ToolTip.timeout: 5000
    ToolTip.visible: !enabled && hovered
    ToolTip.text: qsTr("Lima isn't ready yet!")
  }
  
  
  
}

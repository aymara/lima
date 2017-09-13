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
import QtQuick.Controls 2.2

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

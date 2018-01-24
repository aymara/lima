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
import QtQuick.Controls 2.2

//! Simple scrollable widget to edit text.

// There is a problem with the TextView widget, so this is used instead.

Rectangle {


  property alias text: ttextarea.text
  property alias textFormat: ttextarea.textFormat
  property alias wrapMode: ttextarea.wrapMode
  property alias showMenu: menu.visible
  property alias readOnly: ttextarea.readOnly

  anchors.fill: parent

  Column {

    anchors.topMargin: 10
    anchors.fill: parent

    Rectangle {
      id: menu

      height: 20
      visible: false
      width: parent.width
      anchors.margins: 3
      color: "#eeeeee"

    }

    Rectangle {

      width: parent.width
      height: 1
      color: "#dddddd"
      visible: menu.visible
    }

    ScrollView {
      id: scv
      height: parent.height - (menu.visible ? menu.height : 0)
      width: parent.width
      clip: true
      ScrollBar.vertical.policy: scv.contentHeight > scv.height 
                                      ? ScrollBar.AlwaysOn 
                                      : ScrollBar.AsNeeded
      ScrollBar.horizontal.policy: scv.contentWidth > scv.width 
                                      ? ScrollBar.AlwaysOn 
                                      : ScrollBar.AsNeeded

      TextArea {
        id: ttextarea

        // this creates a binding loop property error : this might come from 
        // Qt itself though
        width: implicitWidth
        height: implicitHeight
        selectByMouse: true
        
        wrapMode: Text.WordWrap
//        textFormat: TextEdit.RichText

        selectionColor: "blue"
        selectedTextColor: "white"
        
      }
    }
  }

}


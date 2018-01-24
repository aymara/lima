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
import QtQml.Models 2.2
import "../scripts/DynamicObjectManager.js" as Dom

//! A generic QML element with a list of switchs. Test widget

Rectangle {
  id: switchPanel
  border.width: 2
  function setModel(mdl) {
    Dom.createComponent(mdl, switchPanel)
    if (Dom.success) {
      gridview.model = Dom.obj
    }
  }

  //! get the boolean by switch name
  function getSwitch(name) {

  }

  /// sample model
  ListModel {
    id: lmodel
    ListElement {
      name:"Hello world"
      value: false
    }
    ListElement {
      name:"ConllHandler"
      value: false
    }
    ListElement {
      name:"DotParser"
      value: true
    }
    ListElement {
     name:"XML Raw"
     value: true
    }ListElement {
      name:"XML Raw"
      value: true
     }ListElement {
      name:"XML Raw"
      value: true
     }ListElement {
      name:"XML Raw"
      value: true
     }ListElement {
      name:"XML Raw"
      value: true
     }ListElement {
      name:"XML Raw"
      value: true
     }ListElement {
      name:"XML Raw"
      value: true
     }ListElement {
      name:"XML Raw"
      value: true
     }ListElement {
      name:"XML Raw"
      value: true
     }
  }

  GridView {
    id:gridview
    model: lmodel
    cellWidth: gridview.width/4
    cellHeight: 50
    anchors.fill: parent
    delegate : Switch {
      id: control
      text: name
      checked: value
    }
  }

}

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

/*!
  This widget allow a simple combo box to choose from multiple options.
  An option is stored as a key->value : key E keys && value E model
  - 'value' is meant to be the displayed text for option O. This string
  would technically need to be translated with the qsTr flag.
  - 'key' the code name for this option.

  example : fr -> Francais(/French/etc.)
  */

Rectangle {
  id: socb

  property alias currentIndex: cbbox.currentIndex
  property string name: "Aleau"

  //! The strings to be displayed in the comboBox widget
  property var model: ['a','b']

  //! The corresponding keys
  property var keys: []

  function getCurrentItem() {
    return model[currentIndex]
  }

  function getCurrentItemKey() {
    return keys[currentIndex]
  }

  signal selected()

  onModelChanged: {
      cbbox.model = model
  }

  height: parent.height
  color: "transparent"

  Column {

    anchors.fill: parent
    spacing: 2

    Text {
      id: nameLabel
      y: -5
      visible: text
      text: name
      font.pointSize: 9
      height: 15
    }

    ComboBox {
      id: cbbox

      model: socb.model
      width: parent.width
      height: parent.height - nameLabel.height

      onCurrentIndexChanged: {
        selected()
      }
    }
  }

}

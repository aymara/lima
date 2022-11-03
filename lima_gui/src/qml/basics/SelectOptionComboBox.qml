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

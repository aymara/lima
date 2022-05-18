// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/*!
  @author   Jocelyn Vernay
  @date     Wed, September 06 2017
  */

import QtQuick 2.5
import QtQuick.Controls 1.4

//! Simple scrollable widget to edit text.

// There is a problem with the TextView widget, so this is used instead.

Rectangle {


  property alias text: ttextarea.text
  property alias textFormat: ttextarea.textFormat
  property alias wrapMode: ttextarea.wrapMode
//   property alias showMenu: menu.visible
  property alias readOnly: ttextarea.readOnly

  anchors.fill: parent

//   Column {
// 
//     anchors.topMargin: 10
//     anchors.fill: parent
// 
//     Rectangle {
//       id: menu
// 
//       height: 20
//       visible: true
//       width: parent.width
//       anchors.margins: 3
//       color: "#eeeeee"
// 
//     }

    TextArea {
      id: ttextarea

      anchors.fill: parent
      height: parent.height - (menu.visible ? menu.height : 0)
      width: parent.width
    }
//   }
}



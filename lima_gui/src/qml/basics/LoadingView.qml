// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/*!
  @author   Jocelyn Vernay
  @date     Wed, September 06 2017
  */

import QtQuick 2.5

//! Simple widget with a loading gif to shows the loading

Rectangle {

  width: animation.width
  height: animation.height + 8
  anchors.fill: parent
  
  AnimatedImage {
    id: animation;

    source: "qrc:///qml/resources/Spinner.gif";
    anchors.centerIn: parent
    scale: 0.5
  }
  
}

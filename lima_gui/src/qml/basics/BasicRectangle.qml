// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/*!
  @author   Jocelyn Vernay
  @date     Wed, September 06 2017
  */

import QtQuick 2.0
import "../scripts/colors.js" as Colors

//! Simple random colored rect for tests

Rectangle {
  
  color: "#ee00eeaa"
  
  Component.onCompleted: {
    color = Colors.randomColor()
  }
  
  
}

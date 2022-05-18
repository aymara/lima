// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/// Handy functions

/// generates a bright random color
function randomColor2() {
  var str = "#"
  for (var i = 0; i < 6; i++) {
    str = str + String.fromCharCode(97 + Math.floor((Math.random() * 5) + 0));
  }
  str = str + "aa";
  //console.log(str);
  return str;
}

/// returns a random char for the randomColor (hex format) function
function randomChar() {
  var i = Math.floor((Math.random() * 5) + 0);
  return String.fromCharCode(97 + i);
}

/// returns a random color in hex format
function randomColor() {
  var str = "#"
  for (var i = 0; i < 6; i++) {
    str = str + randomChar()
  }
  return str;
}

/// brightens a color (hex format)
/// doesn't change alpha
function brighter(color) {
  return Qt.lighter(color)
}

/// darkens a color (hex format)
/// doesn't change alpha
function darker(color) {
  return Qt.darker(color)
}

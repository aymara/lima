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

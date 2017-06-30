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

function randomChar() {
  var i = Math.floor((Math.random() * 5) + 0);
  return String.fromCharCode(97 + i);
}

function randomColor() {
  var str = "#"
  for (var i = 0; i < 6; i++) {
    str = str + randomChar()
  }
  //str = str + "aa";
  //console.log(str);
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

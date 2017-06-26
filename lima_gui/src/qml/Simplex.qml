import QtQuick 2.0
import QtQuick.Controls 1.4
import "script.js" as Da

Rectangle {
  id: simplex_rect
  width: 100
  height: 100
  color: "lightgreen"
  
  Button {
    text: "Simplex"
    onClicked: {
      parent.destroy()
    }
  }
}

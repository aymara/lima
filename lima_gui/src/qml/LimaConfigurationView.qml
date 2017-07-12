import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import "basics"

Popup {
  id: popup

  width: 800
  height: 600
  x:app_window.width/2 - width/2
  y:app_window.height/2 - height/2

  SwitchPanel {
    anchors.fill: parent

  }
}

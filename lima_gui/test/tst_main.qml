import QtQuick 2.6
import QtTest 1.1
import QtQuick.Controls 1.4
import "../src/qml"

Item {
  Button {
    id: button
    onClicked: text = "Clicked"
    TestCase {
        name: "ClickTest"
        when: windowShown
        function test_click() {
            button.clicked();
            compare(button.text, "Clicked");
        }
    }
  }

  EditMenu {
    
  }
}

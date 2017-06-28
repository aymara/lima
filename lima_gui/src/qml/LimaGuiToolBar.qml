import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

ToolBar {
  
  background: Rectangle {
    color: "#eeeeee"
  }
  
  RowLayout {
    ToolButton {
      text: "New"
      Image {
        width: 30
        height: 30
        //source: "55.png"
      }
      //iconSource: "new.png"
    }
    ToolButton {
      text:"Open"
      //iconSource: "open.png"
    }
    ToolButton {
      text:"Save"
      //iconSource: "save-as.png"
    }
    Item { Layout.fillWidth: true }
    
    ToolButton {
      text: "hello"
    }
    
    ToolButton {
      text: "⋮"
    }
  }
}

import QtQuick 2.7
import QtQuick.Controls 1.4 as C1
import QtQuick.Controls 2.2 as C2
import QtQuick.Controls.Styles 1.4
import "basics"
import "scripts/DynamicObjectManager.js" as Dom

/// Figures a view with tabs

C1.TabView {
  id: main_tab_view

  function addTab(name, contentItemSrc) {
    
    Dom.createComponent("basics/Tab.qml", main_tab_view);
    Dom.obj.title = name;
//    currentIndex = Dom.obj.index;
    Dom.createComponent(contentItemSrc, Dom.obj);
    
    // bad design, but returns a reference on the contentItem
    if (Dom.success) {
      return Dom.obj;
    }
    else {
      return null;
    }
  }

  function currentTab() {
    return getTab(currentIndex)
  }
  
  anchors.margins: 10

//  style: TabViewStyle {
//    frameOverlap: 1
//    tab: Rectangle {
//        anchors.margins: 3
//        color: styleData.selected ? "white" : "#aaaaaa"
//        border.color:  styleData.selected ? "steelblue" : "gray"
//        radius: 3
//        y: 2 // to hide bottom border
//        implicitWidth: Math.max(text.width + 4, 80) + 40
//        implicitHeight: 30
//        Text {
//            id: text
//            anchors.centerIn: parent
//            text: styleData.title
//            color: styleData.selected ? "black" : "white"
//        }

////        CloseButton {
////          anchors.right: parent.right
////          width: 20; height: 20

////          onClicked: {
////            parent.closed()
////          }
////        }
//      }
//      frame: Rectangle { border.width: 1; border.color:"steelblue" }
//  }

//   C1.Tab {
//     title: "analyze file"
//     
//     SelectFileMenu {
//       
//       onTriggered: {
//         
//         analyzeFileFromUrl(fileUrl)
//       }
//       text: "Analyser"
//     }
//     
//   }
//   
//   C1.Tab {
//     title: "analyze text"
//     
//     AnalyzeTextWidget {
//       
//     }
//     
//   }
//   
//   C1.Tab {
//     title: "file1.txt"
//     
//     Rectangle {
//       color: "transparent"
//       C1.TextArea {
//         font.family: "Hack"
//         anchors.fill: parent
//         textFormat: TextEdit.RichText
//         wrapMode: TextEdit.WordWrap
//       }
//       
//     }
//   }
//   
//   C1.Tab {
//     title: "tabbedview"
//     
//     
//   }
}

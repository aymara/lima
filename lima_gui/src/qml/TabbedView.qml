import QtQuick 2.7
import QtQuick.Controls 1.4 as C1
import QtQuick.Controls 2.2 as C2
import QtQuick.Controls.Styles 1.4

import "basics"
import "styles"

import "scripts/DynamicObjectManager.js" as Dom

/// Figures a view with tabs

C1.TabView {
  id: tabView

  property var items: []

  function addTabFromSource(name, contentItemSrc) {

    Dom.createComponent("basics/Tab.qml", tabView);
    Dom.obj.title = name;
//    currentIndex = Dom.obj.index;
    Dom.createComponent(contentItemSrc, Dom.obj);
    //addTab(name,Dom.obj)
    currentIndex = count - 1
    if (Dom.success) {
      items.push(Dom.obj)
      return Dom.obj;
    }
    else {
      return null;
    }
  }

  function currentTab() {
    return getTab(currentIndex)
  }

  function closeTab(index) {
    if (index >= 0 && index < count) {

      removeTab(index)
    }
  }

  style: TabbedViewStyle {}
//  style: TabViewStyle {

//    frameOverlap: 1
//    tabsMovable: true
//    tabOverlap : -2

//    tab: Rectangle {

//        anchors.margins: 3
//        color: styleData.selected ? "#eeeeee" : "#cccccc"
////        border.color:  styleData.selected ? "lightblue" : "lightgray"
//        border.width: 1
//        border.color: "#bbbbbb"
//        radius: 3
//        y:2
//        implicitWidth: Math.max(text.width + 4, 80)  + 20 //+ 40 + closebtn.width
//        implicitHeight: 30

//        Text {
//            id: text

//            anchors.centerIn: parent
//            text: styleData.title
//            elide: Text.ElideMiddle
//            color: styleData.selected ? "black" : "black"
//        }

////        MouseArea {
////          onEntered: {
////            parent.color = "red"
////          }
////          onExited: {
////            parent.color = "blue"
////          }
////          anchors.fill:parent
////        }

////        CloseButton {
////          id:closebtn
////          anchors.right: parent.right
////          width: 20; height: 20

////          onClicked: {

////            closeWorkView(styleData.index)
////          }
////        }
//      }

//      frame: Rectangle {

//        border.width: 1;
//        border.color:"#bbbbbb";
//        color:"#eeeeee"

//      }
//  }

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

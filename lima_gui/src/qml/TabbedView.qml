import QtQuick 2.7
import QtQuick.Controls 1.4 as C1
import QtQuick.Controls 2.2 as C2
import "DynamicObjectManager.js" as Dom

C1.TabView {
  id: main_tab_view
  
  function addTab(name, contentItemSrc) {
    Dom.createComponent("basics/Tab.qml", main_tab_view);
    Dom.obj.title = name;
    Dom.createComponent(contentItemSrc, Dom.obj);
    
  }
  
  anchors.margins: 10
  
  C1.Tab {
    title: "analyze file"
    
    SelectFileMenu {
      
      onTriggered: {
        
        textAnalyzer.analyzeFileFromUrl(fileUrl)
      }
      text: "Analyser"
    }
    
  }
  
  C1.Tab {
    title: "analyze text"
    
    AnalyzeTextWidget {
      
    }
    
  }
  
  C1.Tab {
    title: "file1.txt"
    
    Rectangle {
      color: "transparent"
      C1.TextArea {
        font.family: "Hack"
        anchors.fill: parent
        textFormat: TextEdit.RichText
        wrapMode: TextEdit.WordWrap
      }
      
    }
  }
  
  C1.Tab {
    title: "tabbedview"
    
    
  }
}

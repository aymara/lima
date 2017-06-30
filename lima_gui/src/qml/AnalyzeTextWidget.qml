import QtQuick 2.0
import QtQuick.Controls 1.4

GroupBox {
  id: analyzeTextWidget
  
  anchors.fill: parent
  
  title: "Analyser du texte"
  
  TextArea {
    id: text_bunch
    
    width: parent.width - 100
    height: parent.height - 100
    
    anchors.margins: 5
  }
  
  Button {
    text:"Analyze Text"
    onClicked: {
      analyzeText(text_bunch.text)
    }
    
    anchors.top: text_bunch.bottom
  }
  
  
  
}

import QtQuick 2.0
import QtQuick.Controls 1.4

GroupBox {
  id: analyzeTextWidget
  
  width: 400
  height: 100
  
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
      textAnalyzer.text = text_bunch.text
      textAnalyzer.analyzeText()
    }
    
    anchors.top: text_bunch.bottom
  }
  
  
  
}

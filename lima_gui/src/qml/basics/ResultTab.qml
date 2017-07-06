import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4


import "../scripts/DynamicObjectManager.js" as Dom

/// This display the results on an analysis (only CONLL for now)
/// When attempting to analyze, lima_gui first creates the result tab
/// <it should only creates it if the analysis is possible>
/// and then creates a thread in which the analysis does its work.
/// The thread then notifies this QObject by sendint it the results
/// (displayResults)

Rectangle {
  id: result_tab
  anchors.fill: parent
  clip:true
//  color: status ? status == 1 ? "cyan" : "red" : "yellow"

  property string text: "<notext>"
  property string raw: ""
  property string format:""
  property string formatsToLoad:"text|table"
  /// text, table, ...

  // 0 = not active, 1 = analysis running, 2 = analysis over
  property int status: 0

  function displayResults(output) {
    raw = output
    color = "transparent"
    setFormat(format !== "" ? format : "table")

//    console.log("SUCCESS");
//    Dom.createComponent("CONLLTableView.qml", result_tab);
  }

  function reset() {
    format = ""
    Dom.popObject();
    Dom.createComponent("basics/LoadingView.qml", result_tab);
  }

  function setFormat(f) {
    if (f !== format) {
      format = f
      Dom.popObject();
      switch(format) {
      case "text":
        Dom.createComponent("basics/TextView.qml", contentRect);
        Dom.obj.text = raw;
        break;
      case "table":
        Dom.createComponent("ConllTableView.qml", contentRect);
        if (Dom.success) {
          Dom.obj.loadModel(raw)
        }

        break;
      case "detailed_sentences":

        break;
      default:
        console.log(format ," : this format is not supported.");
        break;
      }
    }
  }

  Component.onCompleted: {
   //if (format !== "") Qt.quit()
    reset()

  }

  ToolBar {

    id: toolbar
    height: 25
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    enabled: format != "" ? true : false

    ComboBox {

      width: 100
      height: parent.height
      anchors.margins: 5

      textRole: "text"

      model: ListModel {
        id:cbmodel
        ListElement { name:"table"; text:"Table"}
        ListElement { name:"text"; text:"Texte" }
      }

      onCurrentIndexChanged: {
        setFormat(model.get(currentIndex).name)
      }

    }
  }

  Rectangle {
    id: contentRect
    width: parent.width
    height: parent.height - toolbar.height
    y: toolbar.height

  }
}

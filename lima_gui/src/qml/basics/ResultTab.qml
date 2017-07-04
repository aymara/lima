import QtQuick 2.7
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
  
  property string text: "<notext>"

  function displayResults(output) {
    color = "transparent"
    Dom.popObject();
    Dom.createComponent("basics/TextView.qml", result_tab);
    Dom.obj.text = output;
//    console.log("SUCCESS");
//    Dom.createComponent("CONLLTableView.qml", result_tab);
  }

  Component.onCompleted: {
    Dom.createComponent("basics/LoadingView.qml", result_tab);
  }
}

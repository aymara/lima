/*!
  @author   Jocelyn Vernay
  @date     Wed, September 06 2017
  */

import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import "scripts/DynamicObjectManager.js" as Dom

/*! This is the widget contained in a tab for the main view.
* It contains the text view and the result view.
* To make it more flexible, the views are created dynamically
* via the 'Dom' script.
*/
Rectangle {
  id:workview

  // properties

  // the idea here was to declare these as 'static' properties, like an enum.
  // you can't do that this way in QML.
  // types 'enum'
//  property int Default: 0
//  property int Text: 1
//  property int SelectFile: 2
//  property int OpenFile: 3

  //! The name of the workview <-> the name of the tab
  property string title: ""

  //! The type of the workview ({Default, Text, SelectFile, OpenFile})
  //! This is used for the 'indiscriminateAnalyze' function, which
  //! detect the type and call textAnalyzer.analyze accordingly (analyzeText if Text, etc.)
  property string type: ""

  //! The idea here was to allow the tabs to show when they have completed their analysis
  property int status: resultsItem.length ? resultsItem[0].status : 0

  //! The language selected for this workview
  //!(the consensus now would be to have the analysis bar stored in this widget rather than in the main appWindow)
  property int languageIndex: 0
  property int formatIndex: 0

  //! workarounds as you can't easily store references to objects other than in a list.
  property var dataItem: []
  property var resultsItem: []


  //! load dynamically the object from 'src' and make it a child of the data_view Rectangle.
  function setDataView(src) {
    Dom.createComponent(src,data_view)
    if (Dom.success) {
      if (dataItem.length) {
        dataItem[0].destroy();
        dataItem = [];
      }
      dataItem.push(Dom.obj)
      Dom.obj.focus = true
    }
  }

  //! load dynamically the object from 'src' and make it a child of the result_view Rectangle.
  function setResultView(src) {
    if (src !== "") {
      result_view.show()
      Dom.createComponent(src,result_view)
      if (Dom.success) {
        if (resultsItem.length) {
          resultsItem[0].destroy();
          resultsItem = [];
        }

        resultsItem.push(Dom.obj)
      }
    }
    else {
      result_view.hide()
    }
  }

  //! returns a reference to the data view.
  function getDataView() {
    if (dataItem.length) {
      return dataItem[0];
    }

    else {
      return null;
    }
  }

  //! returns a reference to the result view.
  function getResultView() {
    if (resultsItem.length) {
      return resultsItem[0];
    }

    else {
      return null;
    }
  }

  function status() {
    if (resultsItem.length) {
      return resultsItem[0].status
    }
    else {
      return 0;
    }
  }

  anchors.fill: parent
  color:"transparent"

  // content

  SplitView {

    anchors.margins: 1
    orientation: Qt.Horizontal
    anchors.fill: parent

    handleDelegate: Rectangle {
      height: parent.height
      width: 3
      color: "transparent"
    }

    // data view canvas
    Rectangle {
      id: data_view

      width: workview.width/2
      Layout.fillWidth: true
      color:"transparent"
      radius: 3


    }

    // result view canvas
    Rectangle {
      id: result_view

      function hide() {
        visible = false
      }

      function show() {
        visible = true
      }

      visible:false
      width: workview.width/2
      radius: 3
    }

  }

  


}

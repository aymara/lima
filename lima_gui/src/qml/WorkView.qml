import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import "scripts/DynamicObjectManager.js" as Dom

Rectangle {
  id:workview

  /// properties

  /// types 'enum'
//  property int Default: 0
//  property int Text: 1
//  property int SelectFile: 2
//  property int OpenFile: 3

  property string title: ""
  property string type: ""
  property int status: resultsItem.length ? resultsItem[0].status : 0

  property int languageIndex: 0
  property int formatIndex: 0

  /// workaround as you can't easily store references to objects other than in a list
  property var dataItem: []
  property var resultsItem: []

  /// functions

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

  function getDataView() {
    if (dataItem.length) {
      return dataItem[0];
    }

    else {
      return null;
    }
  }

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

  /// content

  SplitView {

    anchors.margins: 1
    orientation: Qt.Horizontal
    anchors.fill: parent

    handleDelegate: Rectangle {
      height: parent.height
      width: 3
      color: "transparent"
    }

    /// ###
    Rectangle {
      id: data_view

      width: workview.width/2
      Layout.fillWidth: true
      color:"transparent"
      radius: 3


    }

    /// ###
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

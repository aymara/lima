import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4

// import "scripts/DynamicObjectManager.js" as Dom

import "basics"


/// Now, we need to take a static approach (as the dynamic one will eventually lead to memory leaks)

Rectangle {
  id: result_tab

  property string text: "<notext>"
  property string raw: ""
  property string format:""
  // 0 = not active, 1 = analysis running, 2 = analysis over
  property int status: 0

  // add your views references here
  property var views: [myconlltableview, mytextview, myloadingview, myNamedEntitiesView]

  function displayResults(input, output) {
    text = input
    raw = output
    color = "transparent"
    status = 2
    console.log("ResultView::displayResults")
    setFormat(format !== "" ? format : "table")

//    console.log("SUCCESS");
//    Dom.createComponent("CONLLTableView.qml", result_tab);
  }

  function reset() {
//    format = ""
//    Dom.popObject();
//    Dom.createComponent("basics/LoadingView.qml", result_tab);
    format = ""
    console.log("ResultView::reset")
    hideAll()
    myloadingview.visible = true
    status = 1
  }

  function hideAll() {
    for (var i=0; i<views.length; i++) {
      views[i].visible = false
    }
  }

  function setFormat(f) {
    console.log("ResultView::setFormat(", f, ")")
    hideAll()
    if (f !== format) {
      format = f
      //Dom.popObject();
      switch(format) {
      case "text":
//        Dom.createComponent("basics/TextView.qml", contentRect);
//        Dom.obj.text = raw;
        mytextview.visible = true
        mytextview.text = raw;
        break;
      case "table":
//        Dom.createComponent("ConllTableView.qml", contentRect);
//        if (Dom.success) {
//          Dom.obj.loadModel(raw)
//        }
        myconlltableview.visible = true
        myconlltableview.loadModel(raw)
        break;
      case "NE":
        myNamedEntitiesView.visible = true
        myNamedEntitiesView.input(text, raw)

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

  anchors.fill: parent
  clip:true
//  color: status ? status == 1 ? "cyan" : "red" : "yellow"
  /// text, table, ...


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
        ListElement { name:"table"; text: qsTr("Table") }
        ListElement { name:"text";  text:qsTr("Text") }
        ListElement { name:"NE";    text:qsTr("Named entities"); }
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

    // All specific results views go here

    LoadingView {
      id: myloadingview
      visible: true

    }

    TextEditor {
      id:mytextview

      showMenu: false
      wrapMode: TextEdit.NoWrap
      visible: false

    }

    ConllTableView {
      id: myconlltableview
      visible: false

    }

    NamedEntitiesView {
      id: myNamedEntitiesView
      visible: false
    }

  }

}

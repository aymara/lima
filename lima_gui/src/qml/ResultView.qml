import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4

// import "scripts/DynamicObjectManager.js" as Dom

import "basics"


/// Now, we need to take a static approach (as the dynamic one will eventually lead to memory leaks)

/// There is a bug with the combo box : the idea is that changing the combo box index will change
/// the format property of the result view (obviously), and changing the format via setFormat() will
/// consequently update the combo box index. However, it does not work as intended.

/// formatToShow allows the view to pick the choice of the format before displayResults is called by the
/// model. It works fine for a first analysis, but if you try to start a new analysis over the previous one,
/// the format property will get stuck to the combo box index value rather than use formatToShow

Rectangle {
  id: result_tab

  property string text: "<notext>"
  property string raw: ""
  property string format:""
  property string formatToShow: "table"
  property var formats: ["text", "table", "NE", "graph"]
  property var formatNames: [qsTr("Text"), qsTr("Table"), qsTr("Named entities"), qsTr("Graph")]
  // 0 = not active, 1 = analysis running, 2 = analysis over
  property int status: 0

  // add your views references here
  property var views: [myconlltableview, mytextview, myloadingview, myNamedEntitiesView]

  function displayResults(input, output) {
    visible = true
    text = input
    raw = output
    color = "transparent"
    status = 2
    console.log("ResultView::displayResults")
    setFormat(format !== "" ? format : formatToShow)
    toolbar.enabled = true

//    console.log("SUCCESS");
//    Dom.createComponent("CONLLTableView.qml", result_tab);
  }

  function reset() {
//    format = ""
//    Dom.popObject();
//    Dom.createComponent("basics/LoadingView.qml", result_tab);
    format = ""
    formatToShow = "table"
    console.log("ResultView::reset")
    hideAll()
    for (var i = 0; i<views.length; i++) {
      views[i].loaded = false
    }

    myloadingview.visible = true
    status = 1
    toolbar.enabled = false
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
        if (!mytextview.loaded) {
          mytextview.text = raw;
          mytextview.loaded = true
        }

        break;
      case "table":
//        Dom.createComponent("ConllTableView.qml", contentRect);
//        if (Dom.success) {
//          Dom.obj.loadModel(raw)
//        }
        myconlltableview.visible = true
        if (!myconlltableview.loaded) {
          myconlltableview.loaded = true
          myconlltableview.loadModel(raw)
        }

        break;
      case "NE":
        myNamedEntitiesView.visible = true
        if (!myNamedEntitiesView.loaded) {
          myNamedEntitiesView.loaded = true
          myNamedEntitiesView.input(text, raw)
        }

        break;
      default:
        console.log(format ," : this format is not supported.");
        break;
      }
    }
  }

//  onFormatChanged: {
//    console.log("parent::onFormatChanged::despondent")
//    formatbox.currentIndex= (format !== "" ? formats.indexOf(format) : 0)
//  }

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

    height: 30
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
//    enabled: parent.format !== ""

    ComboBox {
      id: formatbox

      width: 200
      x: 5
      y: 5
      height: parent.height - 5
      anchors.margins: 5

//      currentIndex: format != "" ? formats.indexOf(format) : 0

//      textRole: "text"

//      model: ListModel {
//        id:cbmodel
//        ListElement { name: formats[0]; text: qsTr("Table") }
//        ListElement { name: formats[1];  text:qsTr("Text") }
//        ListElement { name: formats[2];    text:qsTr("Named entities"); }
//        ListElement { name: formats[3]; text:qsTr("Graph"); }
//      }

      model: formatNames

      currentIndex: (format !== "" ? formats.indexOf(format) : 0)

      onCurrentIndexChanged: {
//        setFormat(model.get(currentIndex).name)
        console.log("formatBox::onCurrentindexChanged::despondent")
        setFormat(formats[currentIndex]);
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

      property bool loaded: false
      visible: true

    }

    TextEditor {
      id:mytextview

      property bool loaded: false
      showMenu: false
      wrapMode: TextEdit.NoWrap
      visible: false

    }

    ConllTableView {
      id: myconlltableview

      property bool loaded: false
      visible: false

    }

    NamedEntitiesView {
      id: myNamedEntitiesView

      property bool loaded: false
      visible: false
    }

    // GraphView {
    //
    // }

  }

}

import QtQuick 2.7
import QtQuick.Controls 2.2 as C2
import QtQuick.Controls 1.4 as C1
import "scripts/DynamicObjectManager.js" as Dom

Rectangle {
  id: workspace


  color:"#555555"

  property var works: []

  function addWorkTab(name, type, dsrc,  rsrc) {
//    Dom.createComponent("basics/Tab.qml", tabview);
//    Dom.obj.title = name;
//    Dom.createComponent("WorkView.qml",Dom.obj);
//    if (Dom.success) {
//      Dom.obj.setDataView(dsrc);
//      Dom.obj.setResultView(rsrc);
//    }

    tabview.visible = true

    var wv = tabview.addTabFromSource(name,"WorkView.qml")
    if (wv === null) {
      console.log("Couldn't create a new tab/workview");
    }
    else {
      wv.setDataView(dsrc);
      wv.setResultView(rsrc);
      wv.type = type;
      wv.title = name;
      works.push(wv);
    }
    return wv;
  }

  function getCurrentWorkView() {
    if (works.length) {
      return works[tabview.currentIndex];
    }
    else {
      return null;
    }
  }

  function getWorkViewByTitle(title) {
    for (var i = 0; i < works.length; i++) {
      if (title === works[i].title) {
        return works[i];
      }
    }
    return null;
  }

  function count() {
    return works.length;
  }

  TabbedView {
    visible: false
    id:tabview
    anchors.fill: parent
    anchors {topMargin: 5; leftMargin:1; rightMargin: 1}
  }

}

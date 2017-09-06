/*!
  @author   Jocelyn Vernay
  @date     Wed, September 06 2017
  */

import QtQuick 2.7
import QtQuick.Controls 2.2 as C2
import QtQuick.Controls 1.4 as C1
import "scripts/DynamicObjectManager.js" as Dom

//! Main view with tabs; has methods to gain access the work views

Rectangle {
  id: workspace

  //! List of workviews
  property var works: []

  //! Creates a new tab.
  //! 'dsrc' and 'rsrc' are the paths of the widgets files that will be used as data and result views.
  function addWorkTab(name, type, dsrc,  rsrc) {

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

  //! returns the workview currently in focus
  function getCurrentWorkView() {
    if (works.length) {
      return works[tabview.currentIndex];
    }
    else {
      return null;
    }
  }

  //! returns the workview from name
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

  function closeWorkView(index) {
    if (index >= 0 && index < works.length) {
      var wv = works[index]
      if (wv.getResultView() !== null) {
        wv.getResultView().visible = false
      }
      if (wv.getDataView() !== null) {
        wv.getDataView().visible = false
      }

      works.splice(index, 1)
      tabview.getTab(index).visible = false
    }
  }

  function addTab(n,c) {
    tabview.addTab(n,c)
  }

  color:"#555555"

  TabbedView {
    id:tabview

    visible: false
    anchors.fill: parent
    anchors { topMargin: 5; leftMargin:0; rightMargin: 0 }
  }

}

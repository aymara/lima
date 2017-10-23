/*
    Copyright 2017 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/
/*!
  @author   Jocelyn Vernay
  @date     Wed, September 06 2017
  */

import QtQuick 2.5
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
    console.debug("TabbedWorkspace.closeWorkView "+index+" ; "+tabview.currentIndex)
    if (index >= 0 && index < works.length) {
      if (index <= tabview.currentIndex) {
        console.debug("TabbedWorkspace.closeWorkView tabview.currentIndex is now "+tabview.currentIndex)
      }
      var wv = works[index]
      if (wv.getResultView() !== null) {
        wv.getResultView().visible = false
      }
      if (wv.getDataView() !== null) {
        wv.getDataView().visible = false
      }

      works.splice(index, 1)
      tabview.getTab(index).visible = false
      tabview.closeTab(index)
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

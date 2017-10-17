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

import QtQuick 2.7
import QtQuick.Controls 1.4 as C1
import QtQuick.Controls.Styles 1.4

import "basics"
import "styles"

import "scripts/DynamicObjectManager.js" as Dom

//! Figures a view with tabs

C1.TabView {
  id: tabView

  property var items: []

  //! Dynamically add a tab with the associated name.
  function addTabFromSource(name, contentItemSrc) {

    Dom.createComponent("basics/Tab.qml", tabView);
    Dom.obj.title = name;
//    currentIndex = Dom.obj.index;
    Dom.createComponent(contentItemSrc, Dom.obj);
    //addTab(name,Dom.obj)
    currentIndex = count - 1
    if (Dom.success) {
      items.push(Dom.obj)
      return Dom.obj;
    }
    else {
      return null;
    }
  }

  function currentTab() {
    return getTab(currentIndex)
  }

  function closeTab(index) {
    console.debug("tabView.closeTab")
    if (index >= 0 && index < count) {

      removeTab(index)
    }
  }

  style: TabbedViewStyle {

  }
}

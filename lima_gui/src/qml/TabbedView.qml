// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/*!
  @author   Jocelyn Vernay
  @date     Wed, September 06 2017
  */

import QtQuick 2.5
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

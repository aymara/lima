// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/*!
  @author   Jocelyn Vernay
  @date     Wed, September 06 2017
  */

import QtQuick 2.5
import QtQuick.Controls.Styles 1.4

/*!
  This is simply the style element for the TabbedView widget.
  */

TabViewStyle {
  property color frameColor: "#999"
  property color fillColor: "#eee"

  frameOverlap: 1
  tabOverlap: -1
  tabsMovable: true

  frame: Rectangle {
    color: "#eee"
    border.color: frameColor
  }

  tab: Rectangle {

//    color: styleData.selected ? fillColor : frameColor
    color: styleData.selected ? "#eeeeee" : "#cccccc"
    implicitWidth: Math.max(text.width + 24, 80) + 8
    implicitHeight: 20

    Rectangle {
      height: 1 ;
      width: parent.width ;
      color: frameColor
    }

    Rectangle {
      height: parent.height ;
      width: 1;
      color: frameColor
    }

    Rectangle {
      x: parent.width -1;
      height: parent.height ;
      width: 1;
      color: frameColor
    }

    Text {
      id: text

      anchors.left: parent.left
      anchors.verticalCenter: parent.verticalCenter
      anchors.leftMargin: 6
      text: styleData.title
      color: styleData.selected ? "black" : "black"
    }

    Rectangle {

      anchors.right: parent.right
      anchors.verticalCenter: parent.verticalCenter
      anchors.rightMargin: 4
      implicitWidth: 16
      implicitHeight: 16
      radius: width/2
      color: control.hovered ? "#eee": "#00ffffff"
      border.color: control.hovered ?"gray":"#00ffffff"

      Image {
        id:closeImg;

        anchors.centerIn: parent ;
        source: "qrc:///qml-old/resources/cross-symbol-n.png";
        scale: 1.05
      }

      MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
          closeImg.source = "qrc:///qml-old/resources/cross-symbol-h.png"
        }
        onExited: {
          closeImg.source = "qrc:///qml-old/resources/cross-symbol-n.png";
        }
        onClicked: {
//          closeImg.source = "qrc:///qml-old/resources/cross-symbol-h.png";
          // deleting the tab
          workspace.closeWorkView(styleData.index);
//           tabView.closeTab(styleData.index);
        }
      }
    }
  }
}

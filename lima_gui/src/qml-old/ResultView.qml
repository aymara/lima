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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import "basics"

/*!
  The result view widget. The principle here is to include a copy of every 
  specific result view, and show only the one currently in focus atop of the 
  others.
  */


/*! There is a bug with the combo box : the idea is that changing the combo box 
 * index will change the format property of the result view (obviously), and 
 * changing the format via setFormat() will consequently update the combo box 
 * index. However, it does not work as intended.
*/

/*! formatToShow allows the view to pick the choice of the format before 
 * displayResults is called by the model. It works fine for a first analysis, 
 * but if you try to start a new analysis over the previous one, the format 
 * property will get stuck to the combo box index value rather than use 
 * formatToShow
*/

/*!To add additional format views, you need to create a file for it, add the 
 * element to the 'contentRect' below, and edit the 'setFormat' method to add 
 * your format (also, add its codename and real name to the formats list) and 
 * the views list
 */
Rectangle {
  id: result_tab

  property string text: "<notext>"
  property string raw: ""

  //! the current format
  property string format:""

  //! the format that will be shown once the analysis is completed. This is 
  //! altered by the corresponding comboBox in the analysis Bar.
  property string formatToShow: "table"

  //! All formats
  property var formats: ["text", 
                         "table", 
                         "NE", 
//                          "graph"
                        ]

  //! The strings displayed in the combo box
  property var formatNames: [qsTr("Text"), 
                             qsTr("Table"), 
                             qsTr("Named Entities"), 
//                              qsTr("Graph"),
                            ]

  // 0 = not active, 1 = analysis running, 2 = analysis over
  property int status: 0

  // add your views references here
  //! The references to all the different result views.
  //! Every time a new analysis is started or a new format selected, those are 
  //! all hidden.
  property var views: [mytextview,
                       myconlltableview, 
                       myNamedEntitiesView,
                       myloadingview]

  //! This function is called from the C++ in AnalysisThread::notifyView 
  //! function.
  //! The use of the 'input' param may be subject to debate.
  function displayResults(input, output) {
    console.debug("result_tab.displayResults input=" + input 
                  + " ; output=" + output
                  + " ; format=" + (format !== "" ? format : formatToShow))
    
    visible = true
    text = input
    raw = output
    color = "transparent"
    status = 2
    setFormat(format !== "" ? format : formatToShow)
    toolbar.enabled = true
  }

  //! hide all views and put the loading view in focus.
  function reset() {
    format = ""
    formatToShow = "table"
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

  //! set the format
  function setFormat(f) {
    console.debug("result_tab.setFormat "+f)
    hideAll()
    format = f

    switch(format) {
      case "text":
        mytextview.visible = true
//         if (!mytextview.loaded) {
          mytextview.text = raw;
          mytextview.loaded = true
//         }

        break;
      case "table":
        myconlltableview.visible = true
//         if (!myconlltableview.loaded) {
          myconlltableview.loaded = true
          myconlltableview.loadModel(raw)
//         }

        break;
      case "NE":
        myNamedEntitiesView.visible = true
//         if (!myNamedEntitiesView.loaded) {
          myNamedEntitiesView.loaded = true
          myNamedEntitiesView.input(text, raw)
//         }

        break;
      default:
        break;
    }
  }

  Component.onCompleted: {
    reset()

  }

  anchors.fill: parent
  clip:true

  ToolBar {
    id: toolbar

    height: 30
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top

    ComboBox {
      id: formatbox

      width: 200
      x: 5
      y: 5
      height: parent.height - 5
      anchors.margins: 5

      model: formatNames

      Component.onCompleted: 
        currentIndex = (format !== "" ? formats.indexOf(format) : 0)

      onCurrentIndexChanged: {
        setFormat(formats[currentIndex]);
      }

    }

  }

  Rectangle {
    id: contentRect

    width: parent.width
    height: parent.height - toolbar.height
    y: toolbar.height

    LoadingView {
      id: myloadingview

      property bool loaded: false
      visible: true

    }

    // All specific results views go here

    TextView {
      id:mytextview

      property bool loaded: false
//       showMenu: false
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

    // and so on

  }

}

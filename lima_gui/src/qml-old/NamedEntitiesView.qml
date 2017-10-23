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
import QtQuick.Layouts 1.3
import integ_cpp 1.0

import "basics"

/*!
  The result view for the Named Entities result format.

  */

Rectangle {
  id:ne_view

  anchors.fill: parent

  //! Pass the output from the LIMA analysis to the ResultParser
  function input(text,conll) {
    console.debug("ne_view.input "+text+" ; " + conll)
    parser.parse(conll)
    typesview.model = parser.getEntityTypes()
    contenttext.text = parser.getHighlightedText()
    console.debug("ne_view.input contenttext:"+contenttext.text)
  }

  //! C++ class. Parse the results and convert it to HTML text.
  NamedEntitiesParser {
    id: parser
  }

  Row {

    anchors.fill: parent
    spacing: 2

    Rectangle {

      width: parent.width - typesview.width
      height: parent.height

      //! The HTML text is shown here.
      TextEditor {
        id: contenttext

        textFormat: Text.RichText
      }
    }

    //! The list of named entity types.
    Rectangle {
      id: typesview

      property alias model: repeater.model

      width: 150
      height: parent.height

      Column {

        anchors.fill: parent

        Repeater {
          id: repeater

          Rectangle {

            height: 20
            width: parent.width
            radius: 2
            color: modelData.split(':')[1]

            Text {
              anchors.centerIn: parent
              text: modelData.split(':')[0]

            }

          }

        }
      }

    }

  }

}

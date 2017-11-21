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
import QtQml.Models 2.2
import QtQuick.Controls.Styles 1.4

// The custom package from C++
import integ_cpp 1.0

//! This is the result view for the conll table format view.

Rectangle {
  id:conlltableview

  property alias model: table_view.model

  //! ConllListModel class imported from C++.
  ConllListModel {
    id: clm
  }
  
  //! call the eponyme method of ConllList
  function loadModel(text) {
    console.debug("conlltableview.loadModel "+text)
    clm.fromText(text)
    model = clm
  }
  
  anchors.fill: parent

  // https://github.com/aymara/lima/wiki/LIMA-User-Manual
//  property alias headers: table_view_repeater.model
  property var headers: ["ID", "FORM", "LEMMA", "CPOSTAG", "POSTAG", "NE", "FEATS", "HEAD", "DEPREL", "PHEAD", "PDEPREL"]


  TableView {
    id: table_view

    alternatingRowColors: false

    anchors.fill: parent
    style : TableViewStyle {

    }


//    model: clm
// Those roles are defined by the 'roleNames' method of the ConllListModel class
    TableViewColumn { role: "id"; title: "ID"; width: 100 }
    TableViewColumn { role: "form"; title: "FORM"; width: 100 }
    TableViewColumn { role: "lemma"; title: "LEMMA"; width: 100 }
    TableViewColumn { role: "cpostag"; title: "CPOSTAG"; width: 100 }
    TableViewColumn { role: "postag"; title: "POSTAG"; width: 100 }
    TableViewColumn { role: "ne"; title: "NE"; width: 100 }
    TableViewColumn { role: "feats"; title: "FEATS"; width: 100 }
    TableViewColumn { role: "head"; title: "HEAD"; width: 100 }
    TableViewColumn { role: "deprel"; title: "DEPREL"; width: 100 }
    TableViewColumn { role: "phead"; title: "PHEAD"; width: 100 }
    TableViewColumn { role: "pdeprel"; title: "PDEPREL"; width: 100 }

  }
}

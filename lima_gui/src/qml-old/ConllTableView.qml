// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

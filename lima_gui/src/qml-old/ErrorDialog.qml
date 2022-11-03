// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/*!
  @author   Gael de Chalendar <gael.de-chalendar@cea.fr>
  @date     Tue, September 19 2017
  */

import QtQuick 2.5
import QtQuick.Dialogs 1.1

MessageDialog {
    id: errorDialog
    title: qsTr("LIMA Error")
    text: qsTr(
        "An error has occured")
    informativeText: qsTr(
        "No more information  is available")
    onAccepted: {
      Qt.quit()
    }
}

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

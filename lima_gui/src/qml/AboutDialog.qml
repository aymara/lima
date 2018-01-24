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
  @date     Fri, September 08 2017
  */

import QtQuick 2.5
import QtQuick.Dialogs 1.1

MessageDialog {
    id: aboutDialog
    title: qsTr("About Lima GUI")
    text: qsTr(
        "Lima GUI is a simple graphical user interface for the LIMA "
        + "Libre Multilingual analyzer.")
    informativeText: qsTr(
        "It is dedicated to beginners or users not at ease with command line "
        + "and other raw XML configuration files. It gives access to LIMA "
        + "basic features in an easy way.<br/><br/>"
        + "Copyright 2017 CEA LIST.<br/><br/>"
        + "Lima GUI is Free Software. It is released under the AGPL 3.0 "
        + "licence (or any later version at your convenience).<br/><br/>"
        + "Authors:<br/><ul>"
        + "<li>Jocelyn Vernay</li>"
        + "<li>Gaël de Chalendar</li></ul>")
    onAccepted: {
    }
}

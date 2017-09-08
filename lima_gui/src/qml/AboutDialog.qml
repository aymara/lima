/*!
  @author   Gael de Chalendar <gael.de-chalendar@cea.fr>
  @date     Fri, September 08 2017
  */

import QtQuick 2.2
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

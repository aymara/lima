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
import QtQuick 2.9
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2 as C2
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1

Dialog {
        id: settingsDialog
        x: Math.round((parent.width - width) / 2)
        y: Math.round(parent.height / 6)
        width: Math.round(Math.min(parent.width, parent.height) / 3 * 2)
//         modal: true
//         focus: true
        title: qsTr("Settings")

        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
//             settings.style = styleBox.displayText
            settingsDialog.close()
        }
        onRejected: {
//             styleBox.currentIndex = styleBox.styleIndex
            settingsDialog.close()
        }

        contentItem: ColumnLayout {
            id: settingsColumn
            spacing: 20

            C2.Label {
                text: qsTr("There is currently nothing to set.")
                color: "#e41e25"
            }
            
//             RowLayout {
//                 spacing: 10
// 
//                 Label {
//                     text: "Style:"
//                 }
// 
//                 ComboBox {
//                     id: styleBox
//                     property int styleIndex: -1
//                     model: ["a", "b"]
//                     Component.onCompleted: {
//                     }
//                     Layout.fillWidth: true
//                 }
//             }
// 
//             Label {
//                 text: "Restart required"
//                 color: "#e41e25"
//                 opacity: styleBox.currentIndex !== styleBox.styleIndex ? 1.0 : 0.0
//                 horizontalAlignment: Label.AlignHCenter
//                 verticalAlignment: Label.AlignVCenter
//                 Layout.fillWidth: true
//                 Layout.fillHeight: true
//             }
        }
    }

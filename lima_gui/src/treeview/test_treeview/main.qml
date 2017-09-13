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
import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.2

import hello 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    TreeModel {
        id: tmodel

    }

    TreeView {
        anchors.fill: parent
        model: tmodel

//        itemDelegate: Item {
//            Text {
//                anchors.fill: parent
//                color: styleData.textColor
//                elide: styleData.elideMode
//                text: styleData.value.indentation + ": " + styleData.value.text
//            }
//        }

        itemDelegate: Item {
            TextEdit {
                anchors.fill: parent
                color: styleData.textColor
                text: styleData.value
            }
        }


        TableViewColumn {
            title: "name"
            role: "name"
        }

        TableViewColumn {
            title: "summary"
            role: "desc"
        }

//        TableViewColumn {
//            title: "Value"
//            role: "checked"
//        }

    }
}

// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

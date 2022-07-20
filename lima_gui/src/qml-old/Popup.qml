// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

import QtQuick 2.0

Rectangle {

    radius: 5
    border.color: "#000000"
    border.width: 2
    smooth: true
    color: "#5e5e5e"
    z:100
    state: "invisible"

    states: [
        State {
            name: "invisible"
            PropertyChanges { target: popup; opacity: 0 }
        },

        State {
            name: "visible"
            PropertyChanges { target: popup; opacity: 1.0 }
        }
    ]

    transitions: Transition {
        NumberAnimation { properties: "opacity"; duration: 100 }
    }

    function toggle() {
        if (state == "visible")
            state = "invisible";
        else
            state = "visible";
    }
}
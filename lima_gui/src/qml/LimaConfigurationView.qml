/*!
  @author   Jocelyn Vernay
  @date     Wed, September 06 2017
  */

import QtQuick 2.7
import QtQuick.Window 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import "basics"

import integ_cpp 1.0

/*!
    The popup widget for the configuration ui.
    Gives white empty look for now.
  */

Popup {
id: popup

    width: 800
    height: 600
    x:app_window.width/2 - width/2
    y:app_window.height/2 - height/2


    ConfigurationTreeModel {
       id: limaconfig


    }

    StackView {
        id: stack

        initialItem: configListView

        anchors.fill: parent

        Rectangle {
            id: configListView

            anchors.fill: parent
            anchors.margins: 20

            border { width:1; color:"black"}

            ColumnLayout {

              anchors.fill: parent

              Text {
                text: qsTr("Nouveau :")
              }

              Rectangle {

                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredHeight:350
                border.width: 1
                border.color: "#aaaaaa"
                radius: 2

                ListModel {
                  id: lmodel
                }

                ListView {
                  id: lview

                  anchors.fill: parent
                  anchors.margins: 2
                  snapMode: ListView.SnapToItem
                  model: lmodel

                  delegate: Label {
                    text: name
                    width: parent.width


                    background : Rectangle {
                      id: bgrec

                      color:"transparent"
                    }

                    MouseArea {
                      anchors.fill: parent

                      onEntered: {
                        //bgrec.color = "#EEEEEEAA"
                      }

                      onExited: {
                        //bgrec.color = "#AAAAEEAA"
                      }

                      onDoubleClicked: {
                        lview.currentIndex = index
                        finalizeBtn.clicked()
                      }

                      onClicked: lview.currentIndex = index
                    }

                  }

                  highlight: Rectangle { color: "lightblue";  }
                  focus: true

                }
              }

              Rectangle {
                id: button_bar

                Layout.fillWidth: true
                Layout.preferredHeight: 40

                Row {

                  anchors.centerIn: parent
                  spacing: 2
                  Button {
                    id: finalizeBtn

                    text: qsTr("Ok")
                    onClicked: {
                      console.log(lview.currentItem.text)
                      lmodel.handle(lview.currentIndex)
                      close()
                    }
                  }

                  Button {
                    id: cancelBtn

                    text: qsTr("Annuler")
                    onClicked: {
                      close()
                    }
                  }

                }

              }

            }

        }

        Rectangle {
            id: editConfigView

            anchors.fill: parent
        }

    }

}


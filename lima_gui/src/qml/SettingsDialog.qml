/*!
  @author   Gael de Chalendar <gael.de-chalendar@cea.fr>
  @date     Fri, September 08 2017
  */
import QtQuick 2.9
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1

Dialog {
        id: settingsDialog
        x: Math.round((parent.width - width) / 2)
        y: Math.round(parent.height / 6)
        width: Math.round(Math.min(parent.width, parent.height) / 3 * 2)
        modal: true
        focus: true
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

            Label {
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

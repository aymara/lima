import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Rectangle {
  id:ne_view

  function setModel(model) {
    typesview.model = model
  }

  function setText(text) {
    contenttext.text = text
  }

  RowLayout {

    anchors.fill: parent
    spacing: 2

    TextView {
      id: contenttext

      width: parent.width - typesview.width
      height: parent.height
    }

    Rectangle {
      id: typesview

      property alias model: repeater.model

      width: 50
      height: parent.height

      Repeater {
        id: repeater
        model: []

        Rectangle {

          anchors.margins: 10
          color: split(metaData)[0]

          Text {

            anchors.fill: parent
            text: split(metaData)[1]

          }

        }

      }

    }

  }

}

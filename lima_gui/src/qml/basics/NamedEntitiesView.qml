import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import integ_cpp 1.0

Rectangle {
  id:ne_view

  anchors.fill: parent

  function input(text,conll) {
    parser.parse(text,conll)
    typesview.model = parser.getEntityTypes()
    contenttext.text = parser.getHighlightedText()
  }


  NamedEntitiesParser {
    id: parser
  }

  Row {

    anchors.fill: parent
    spacing: 2

    Rectangle {

      width: parent.width - typesview.width
      height: parent.height

      TextEditor {
        id: contenttext
//        text: "hello! erg"
        textFormat: Text.RichText
      }
    }

    Rectangle {
      id: typesview

      property alias model: repeater.model

      width: 150
      height: parent.height

      Column {

        anchors.fill: parent

        Repeater {
          id: repeater

          Rectangle {

            height: 20
            width: parent.width
//            y: height*index

//            color: "lightgray"
            radius: 2
            color: modelData.split(':')[1]

            Text {
              anchors.centerIn: parent
              text: modelData.split(':')[0]
//              text: modelData

            }

          }

        }
      }

    }

  }

}

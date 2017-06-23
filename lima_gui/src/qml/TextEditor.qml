
/**
 * \author Jocelyn VERNAY
 * \file main.qml
 * \date June 2017
 */

import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

import integ_cpp 1.0
import "script.js" as Script

/**
 * Main QML File : Basic Text Editor
 */

Window
{
    title: "TextEditor"
    id:app_window
    visible: true
    width: 800
    height: 600
    
    Writer {
      id: writer_data
    }
    
    FileDialog {
      id: file_browser
      onAccepted: {
        writer_data.filepath = fileUrl
        writer_data.loadFile()
        text_editor.text = writer_data.file_content
        text_editor.focus = true
      }
    }

//     Rectangle {
//     z: 2
//     id: main_menu_bar
//     anchors.top: parent.top
//     width: parent.width
//     height: 30
//     
    Row {
        spacing: 2
        
        Button { 
          text:"Save"; 
          id: saveButton
            
          onClicked: {
            writer_data.file_content = text_editor.text
            writer_data.saveFile()
          }
            
        }
        Button { 
          text:"Load";
          id: loadButton
            
          onClicked: {
            file_browser.open()
          }
            
        }
        
        
        
        Button {
            id:whatButton
            text:"What?"
            onClicked: Script.createDialog()
        }
        
//         Button {width:50; height:20; text: "File"}
//         Button {width:50; height:20; text: "Edit"}
//         Button {width:50; height:20; text: "View"}
//         Button {width:50; height:20; text: "Options"}
    }
// }
    Rectangle {
      z: 1
      id: text_rect
      border.color: "gray"
      border.width: 1
      color:"transparent"
      width: parent.width
      y: 40
      height: parent.height - y
      
      
      anchors.margins: 0
        
      Flickable {
        id: text_flick
        anchors.fill: parent
        
        contentWidth: text_editor.width
        contentHeight: text_editor.height
        clip: true
        
        function ensureVisible(r)
        {
          if (contentX >= r.x)
            contentX = r.x;
          else if (contentX+width <= r.x+r.width)
            contentX = r.x+r.width-width;
          if (contentY >= r.y)
            contentY = r.y;
          else if (contentY+height <= r.y+r.height)
            contentY = r.y+r.height-height;
        }
        
        TextArea {
          id: text_editor
          anchors.margins: 0
          focus:true
          wrapMode:TextArea.Wrap
          onCursorRectangleChanged: text_flick.ensureVisible(cursorRectangle)
          // this allows basic html format
          textFormat: TextEdit.RichText
          // https://stackoverflow.com/questions/39440057/qml-how-to-dynamically-chage-the-color-of-a-textarea
          text:"<sss style='background-color: red'> Hello in red! </sss> <sss> Regular hello </sss>"
        }
      }
    }
}

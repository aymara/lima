import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQml.Models 2.2
import "../models"

/// TEST FILE. OBSOLETE.

TableView {
  anchors.centerIn: parent
  anchors.fill: parent
  model: TableModel {
    
  }
  TableViewColumn {
    role: "foo"
    title: "Foo"
    width: 80
  }
  TableViewColumn {
    role: "bar"
    title: "Bar"
    width: 80
  }
  TableViewColumn {
    role: "baz"
    title: "Baz"
    width: 80
  }
}

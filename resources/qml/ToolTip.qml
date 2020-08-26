import QtQuick 2.12

Item {
  id: root

  property string itemId

  width: 100
  height: 48 + 10

  Rectangle {
    anchors.fill: parent

    anchors.margins: 10

    color: "black"

    Text {
      text: Dock.itemClassById(root.itemId)
      color: "white"
    }
  }
}

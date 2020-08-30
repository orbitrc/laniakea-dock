import QtQuick 2.12

Item {
  id: root

  property string itemId

  width: 160
  height: 200

  Rectangle {
    anchors.fill: parent
    anchors.margins: 10

    radius: 10

    Text {
      text: root.itemId
      font.pixelSize: 16
    }
  }
}

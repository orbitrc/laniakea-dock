import QtQuick 2.12

Item {
  id: root

  property string itemId

  width: text.implicitWidth + 20
  height: 48 + 10

  Rectangle {
    anchors.fill: parent

    anchors.margins: 10

    color: "black"

    Text {
      id: text

      text: Dock.itemProperIconName(root.itemId)
      color: "white"
      font.pixelSize: 18
      font.bold: true
    }
  }
}

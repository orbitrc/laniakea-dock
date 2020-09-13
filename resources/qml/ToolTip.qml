import QtQuick 2.12

Item {
  id: root

  property string itemId

  width: text.implicitWidth + 40  // 20 for margins, 20 for paddings.
  height: 48 + 10

  Rectangle {
    anchors.fill: parent

    anchors.margins: 10

    color: "black"
    radius: 6

    Text {
      id: text

      anchors.centerIn: parent
      text: Dock.itemProperIconName(root.itemId)
      color: "white"
      font.pixelSize: 18
      font.bold: true
    }
  }
}

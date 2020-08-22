import QtQuick 2.12

Rectangle {
  id: root

  property int wId
  property string itemId

  width: 64
  height: 64

  color: "grey"

  Text {
    width: root.width

    text: Dock.itemClassById(root.itemId)
    font.pixelSize: 10
    wrapMode: Text.WrapAnywhere
  }
}

import QtQuick 2.12

Rectangle {
  id: root

  property int wId
  property string itemId

  width: 64
  height: 64

  color: "#eff0f4"

  Text {
    width: root.width

    text: Dock.itemClassById(root.itemId)
    font.pixelSize: 10
    wrapMode: Text.WrapAnywhere
  }
  Rectangle {
    anchors.centerIn: parent

    width: 48
    height: 48

    color: isActive() ? "grey" : "transparent"
  }

  function isActive() {
    return root.itemId == Dock.activeWindowItemId;
  }
}

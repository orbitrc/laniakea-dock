import QtQuick 2.12
import QtQuick.Window 2.12

Rectangle {
  width: 640
  height: 64

  Repeater {
    model: Dock.itemIds.length
    Icon {
      x: index * 64
//      wId: Dock.itemIds[index]
      itemId: Dock.itemIds[index]
    }
  }
}

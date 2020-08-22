import QtQuick 2.12
import QtQuick.Window 2.12

Rectangle {
  width: 640
  height: 64

  color: "#a9a9a9"

  Flow {
    spacing: 1

    Repeater {
      model: Dock.itemIds.length
      Icon {
//        wId: Dock.itemIds[index]
        itemId: Dock.itemIds[index]
      }
    }
  }
}

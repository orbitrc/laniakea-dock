import QtQuick 2.12
import QtQuick.Window 2.12

Rectangle {
  width: 640
  height: 64

  Repeater {
    model: Dock.windows.length
    Icon {
      x: index * 64
      wId: Dock.windows[index]
    }
  }
}

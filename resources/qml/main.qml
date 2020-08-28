import QtQuick 2.12
import QtQuick.Window 2.12

Rectangle {
  width: 800
  height: 64

  color: "#a9a9a9"

  Flow {
    spacing: 1

    Repeater {
      id: icons

      model: Dock.itemIds.length
      Icon {
//        wId: Dock.itemIds[index]
        itemId: Dock.itemIds[index]
      }
    }
  }

  // Debug quit button
  Rectangle {
    id: debugQuitButton

    color: "red"

    anchors.right: parent.right

    width: 20
    height: 20

    MouseArea {
      anchors.fill: parent

      onClicked: {
        Qt.quit();
      }
    }
  }

  Connections {
    target: Dock

    onWidgetGeometryChanged: {
      for (let i = 0; i < icons.count; ++i) {
        icons.itemAt(i).updateIconGeometry();
      }
    }
  }
}

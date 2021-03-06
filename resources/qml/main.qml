import QtQuick 2.12
import QtQuick.Window 2.12

Rectangle {
  width: (64 * Dock.items.length) + (iconsLayout.spacing * Dock.items.length) + 20
  height: 64

  color: "#a9a9a9"

  Flow {
    id: iconsLayout

    spacing: 1

    Repeater {
      id: icons

      model: Dock.items.length
      Icon {
        item: Dock.items[index]
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

    function onWidgetGeometryChanged() {
      for (let i = 0; i < icons.count; ++i) {
        icons.itemAt(i).updateIconGeometry();
      }
    }
  }
}

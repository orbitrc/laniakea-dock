import QtQuick 2.12
import QtGraphicalEffects 1.0

Rectangle {
  id: root

  property var item

  width: 64
  height: 64

  color: "#eaeaea"

  Item {
    id: innerShadowSource

    anchors.fill: parent

    Rectangle {
      id: activeIndicator

      anchors.centerIn: parent

      width: 54
      height: 54

      radius: 10

      color: root.color
    }
  }

  InnerShadow {
    id: topLeftShadow

    visible: isActive()

    anchors.fill: innerShadowSource
    source: innerShadowSource
    horizontalOffset: 2
    verticalOffset: 2
    radius: 2
    samples: 16
    color: "#808080"
  }

  InnerShadow {
    id: bottomRightShadow

    visible: isActive()

    anchors.fill: innerShadowSource
    source: innerShadowSource
    horizontalOffset: -2
    verticalOffset: -2
    radius: 2
    samples: 16
    color: "#ffffff"
  }

  Blend {
    visible: isActive()

    anchors.fill: topLeftShadow
    source: topLeftShadow
    foregroundSource: bottomRightShadow
    mode: "average"
  }

  Image {
    id: iconImage

    anchors.centerIn: parent

    width: 48
    height: 48

    source: 'image://icons/' + root.item.id

    cache: false
  }

  ColorIndicator {
    visible: Dock.itemWindowsById(root.item.id).length > 0
    anchors.horizontalCenter: parent.horizontalCenter
    y: root.height - (this.height - 2)
  }

  MouseArea {
    anchors.fill: parent

    hoverEnabled: true
    acceptedButtons: Qt.LeftButton | Qt.RightButton

    onClicked: {
      if (mouse.button === Qt.LeftButton) {
        let wins = Dock.itemWindowsById(root.item.id);
        if (wins.length === 0) {
          Dock.runApplication(root.item.id);
        }
        if (wins.length === 1) {
          Dock.activateWindow(wins[0]);
        }
      } else if (mouse.button === Qt.RightButton) {
        PopUpManager.showContextMenu(root.item.id);
      }
    }

    onEntered: {
      PopUpManager.showToolTip(root.item.id);
    }

    onExited: {
      PopUpManager.hideToolTip();
    }
  }

  function isActive() {
    return root.item.id === Dock.activeWindowItemId;
  }

  function updateIconGeometry() {
    let global = mapToGlobal(0, 0);
    Dock.itemSetIconGeometry(root.itemId, Qt.rect(global.x, global.y, root.width, root.height));
  }
}

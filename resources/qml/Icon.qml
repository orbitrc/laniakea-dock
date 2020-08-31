import QtQuick 2.12
import QtGraphicalEffects 1.0

Rectangle {
  id: root

  property int wId
  property string itemId

  width: 64
  height: 64

  color: "#eaeaea"

  Item {
    id: innerShadowSource

    anchors.fill: parent

    Rectangle {
      id: activeIndicator

      anchors.centerIn: parent

      width: 52
      height: 52

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

    source: 'image://icons/' + root.itemId
  }

  ColorIndicator {
    visible: Dock.itemWindowsById(root.itemId).length > 0
    anchors.horizontalCenter: parent.horizontalCenter
    y: root.height - (this.height - 2)
  }

  MouseArea {
    anchors.fill: parent

    hoverEnabled: true
    acceptedButtons: Qt.LeftButton | Qt.RightButton

    onClicked: {
      if (mouse.button === Qt.LeftButton) {
        let wins = Dock.itemWindowsById(root.itemId);
        if (wins.length === 0) {
          Dock.runApplication(root.itemId);
        }
        if (wins.length === 1) {
          Dock.activateWindow(wins[0]);
        }
      } else if (mouse.button === Qt.RightButton) {
        PopUpManager.showContextMenu(root.itemId);
      }
    }

    onEntered: {
      PopUpManager.showToolTip(root.itemId);
    }

    onExited: {
      PopUpManager.hideToolTip();
    }
  }

  function isActive() {
    return root.itemId == Dock.activeWindowItemId;
  }

  function updateIconGeometry() {
    let global = mapToGlobal(0, 0);
    Dock.itemSetIconGeometry(root.itemId, Qt.rect(global.x, global.y, root.width, root.height));
  }
}

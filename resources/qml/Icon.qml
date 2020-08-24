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
  Text {
    id: debugMessage

    anchors.bottom: parent.bottom

    text: ''
    font.pixelSize: 9
  }

  Rectangle {
    anchors.centerIn: parent

    width: 48
    height: 48

    color: isActive() ? "grey" : "transparent"

    Image {
      id: iconImage

      width: 48
      height: 48

      source: 'image://icons/' + root.itemId
    }
  }
  MouseArea {
    anchors.fill: parent

    onClicked: {
      let wins = Dock.itemWindowsById(root.itemId);
      debugMessage.text = wins.length;
      if (wins.length === 1) {
        Dock.activateWindow(wins[0]);
      }
    }
  }

  function isActive() {
    return root.itemId == Dock.activeWindowItemId;
  }
}

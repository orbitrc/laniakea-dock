import QtQuick 2.12

Item {
  id: root

  property string itemId

  width: 160
  height: 200

  Rectangle {
    anchors.fill: parent
    anchors.margins: 10

    radius: 10

    Flow {
      flow: Flow.TopToBottom

      Text {
        text: root.itemId
        font.pixelSize: 16
      }
      Text {
        text: 'Class: ' + Dock.itemClassById(root.itemId)
        font.pixelSize: 10
      }
      Text {
        text: 'Path: ' + Dock.itemPathById(root.itemId)
        font.pixelSize: 9
      }
      Image {
        id: image

        width: 20
        height: 20
        source: ''
      }
    }
  }

  onItemIdChanged: {
    image.source = 'image://icons/' + root.itemId;
  }
}

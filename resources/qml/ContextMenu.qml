import QtQuick 2.12

Item {
  id: root

  property var item

  width: 160
  height: 200

  Rectangle {
    anchors.fill: parent
    anchors.margins: 10

    radius: 10

    Flow {
      flow: Flow.TopToBottom

      Text {
        text: root.item.id
        font.pixelSize: 16
      }
      Text {
        text: 'Class: ' + Dock.itemClassById(root.item.id)
        font.pixelSize: 10
      }
      Text {
        text: 'Path: ' + root.item.path
        font.pixelSize: 9
      }
      Text {
        text: 'Windows: ' + root.item.windows.length
        font.pixelSize: 10
      }

      Image {
        id: image

        width: 20
        height: 20
        source: ''
      }
    }
  }
}

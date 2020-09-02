import QtQuick 2.12

Item {
  id: root

  property var item: null

  width: 160
  height: 200

  Rectangle {
    anchors.fill: parent
    anchors.margins: 10

    radius: 10

    Flow {
      flow: Flow.TopToBottom

      anchors.fill: parent

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
      Loader {
        id: isPinnedLoader

        width: parent.width
      }
      Loader {
        id: pinUnpinLoader

        width: parent.width
      }
    }
  }

  //==================
  // Components
  //==================
  Component {
    id: isPinnedComponent

    MenuItem {
      text: 'Pinned: ' + root.item.pinned
    }
  }
  Component {
    id: pinUnpinComponent

    MenuItem {
      Flow {
        Rectangle {
          width: 20
          height: 20
          border.color: "black"
          color: (root.item !== null) ? (root.item.pinned ? "red" : "white") : "white"
        }
        Text {
          text: 'Pin/Unpin'
        }
      }
    }
  }

  onItemChanged: {
    if (root.item !== null) {
      isPinnedLoader.sourceComponent = isPinnedComponent;
      pinUnpinLoader.sourceComponent = pinUnpinComponent;
    }
  }
}

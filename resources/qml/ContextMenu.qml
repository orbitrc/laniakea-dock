import QtQuick 2.12

Item {
  id: root

  property var item: null

  width: (clickToDebugLoader.item) ? clickToDebugLoader.item.width : 160
  height: 200

  Rectangle {
    id: menuRect

    anchors.fill: parent
    anchors.margins: 10

    radius: 10
    clip: true
    color: "black"

    Flow {
      flow: Flow.TopToBottom

      anchors.fill: parent

      Text {
        text: (root.item) ? root.item.id : ''
        font.pixelSize: 13
        color: "white"
      }
      Text {
        text: 'Class: ' + (root.item ? Dock.itemClassById(root.item.id) : '')
        font.pixelSize: 10
        color: "white"
      }
      Text {
        text: 'Path: ' + (root.item ? root.item.path : '')
        font.pixelSize: 10
        color: "white"
      }
      Text {
        text: 'Windows: ' + (root.item ? root.item.windows.length : '')
        font.pixelSize: 10
        color: "white"
      }

      Loader {
        id: clickToDebugLoader

        width: (item) ? item.implicitWidth : parent.width
      }
      Loader {
        id: isPinnedLoader

        width: (item) ? item.implicitWidth : parent.width
      }
      Loader {
        id: pinUnpinLoader

        width: (item) ? item.implicitWidth : parent.width
      }
    }
  }

  //==================
  // Components
  //==================
  Component {
    id: clickToDebugComponent

    MenuItem {
      title: 'Click to Debug'
      action: function() {
        print(root.width);
        print(menuRect.width);
        print(this.implicitWidth);
      }
    }
  }
  Component {
    id: isPinnedComponent

    MenuItem {
      title: 'Pinned: ' + root.item.pinned
      disabled: true
    }
  }
  Component {
    id: pinUnpinComponent

    MenuItem {
      title: 'Pin/Unpin'
      checkable: true
      checked: root.item.pinned ? true : false
      action: function() {
        root.item.pinned = !root.item.pinned;
      }
    }
  }

  onItemChanged: {
    if (root.item !== null) {
      clickToDebugLoader.sourceComponent = clickToDebugComponent;
      isPinnedLoader.sourceComponent = isPinnedComponent;
      pinUnpinLoader.sourceComponent = pinUnpinComponent;
    }
  }
}

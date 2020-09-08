import QtQuick 2.12

Item {
  id: root

  property var item: null

  width: (clickToDebug) ? largestWidth() : 160
  height: 280

  Rectangle {
    id: menuRect

    anchors.fill: parent
    anchors.margins: 10

    radius: 10
    clip: true
    color: "black"

    Flow {
      id: menuItems

      flow: Flow.TopToBottom

      anchors.fill: parent

      MenuItem {
        title: (root.item) ? root.item.id : ''
        disabled: true

        width: largestWidth()
      }
      MenuItem {
        title: 'Class: ' + (root.item ? Dock.itemClassById(root.item.id) : '')
        disabled: true

        width: largestWidth()
      }
      MenuItem {
        title: 'Path: ' + (root.item ? root.item.path : '')
        disabled: true

        width: largestWidth()
      }
      MenuItem {
        title: 'Windows: ' + (root.item ? root.item.windows.length : '')

        width: largestWidth()
      }
      MenuItem {
        id: clickToDebug

        title: 'Click to Debug'
        action: function() {
          print(largestWidth());
        }

        width: largestWidth()
      }
      MenuItem {
        title: 'Pinned: ' + (root.item ? root.item.pinned : '')
        disabled: true

        width: largestWidth()
      }
      MenuItem {
        title: 'Pin/Unpin'
        checkable: true
        checked: (root.item) ? (root.item.pinned ? true : false) : ''
        action: function() {
          root.item.pinned = !root.item.pinned;
        }

        width: largestWidth()
      }
      MenuItem {
        title: 'Close'
        action: function() {
          for (let i = 0; i < root.item.windows.length; ++i) {
            Dock.closeWindow(root.item.windows[i]);
          }
        }

        width: largestWidth()
      }
    }
  }

  //==================
  // Components
  //==================
  onItemChanged: {
    if (root.item !== null) {
    }
  }

  function largestWidth() {
    let largest = 0;
    for (let i = 0; i < menuItems.children.length; ++i) {
      let item = menuItems.children[i];
      if (item.implicitWidth > largest) {
        largest = item.implicitWidth;
      }
    }

    return largest;
  }
}

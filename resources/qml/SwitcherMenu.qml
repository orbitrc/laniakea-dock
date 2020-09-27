import QtQuick 2.12

Item {
  id: root

  property var item: null

  width: 160
  height: 200

  Rectangle {
    anchors.fill: parent

    color: 'black'

    Flow {
      id: menuItems

      flow: Flow.TopToBottom

      anchors.fill: parent

      Repeater {
        model: (root.item === null) ? 0 : root.item.windows.length

        MenuItem {
          title: root.item.windows[index]
          action: function() {
            Dock.activateWindow(root.item.windows[index]);
          }

          width: 160
        }
      }
    }
  }
}

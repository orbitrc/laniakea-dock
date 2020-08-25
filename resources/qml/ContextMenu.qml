import QtQuick 2.12

Item {
  id: root

  property string itemId

  width: 100
  height: 100

  Rectangle {
    anchors.fill: parent
    anchors.margins: 10

    radius: 10

    Text {
      text: root.itemId
    }
  }

}

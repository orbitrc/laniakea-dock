import QtQuick 2.12

Rectangle {
  id: root

  property int wId

  width: 64
  height: 64

  color: "grey"

  Text {
    text: '0x' + root.wId.toString(16)
    font.pixelSize: 8
  }
}

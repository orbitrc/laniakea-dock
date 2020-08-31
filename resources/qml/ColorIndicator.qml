import QtQuick 2.12

Rectangle {
  id: background

  width: 10
  height: 10

  radius: 25

  color: "#4CD4DE"

  Rectangle {
    id: foreground

    anchors.fill: parent
    anchors.centerIn: parent
    anchors.margins: 3

    color: "#04FDFE"
  }
}

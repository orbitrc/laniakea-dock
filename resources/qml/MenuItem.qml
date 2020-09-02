import QtQuick 2.12

Rectangle {
  id: root

  property var item: null
  property bool disabled: false
  property string title: ''
  property var action: null
  property bool checkable: false
  property bool checked: false

  height: 32

  color: "cyan"

  states: [
    State {
      name: 'hovered'
      PropertyChanges {
        target: root
        color: "red"
      }
    }
  ]

  Rectangle {
    visible: root.checkable

    width: 20
    height: 20

    border.color: "black"
    color: root.checked ? "red" : "transparent"
  }

  Text {
    x: 30
    text: root.title
  }

  MouseArea {
    id: mouseArea

    anchors.fill: parent

    hoverEnabled: true

    onClicked: {
      if (root.action) {
        root.action();
      }
    }
    onEntered: {
      if (!root.disabled) {
        root.state = 'hovered';
      }
    }
    onExited: {
      root.state = '';
    }
  }
}

import QtQuick 2.12

Rectangle {
  id: root

  property var item: null
  property bool disabled: false
  property string text

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

  Text {
    text: root.text
  }

  MouseArea {
    id: mouseArea

    anchors.fill: parent

    hoverEnabled: true

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

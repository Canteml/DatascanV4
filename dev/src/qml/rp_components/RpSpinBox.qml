import QtQuick 2.0
import QtQuick.Controls 2.15
import Design 1.0

SpinBox {
    id: sb
    value: 20

    leftPadding: down.indicator.width-4

    implicitHeight: 32

    up.indicator: Rectangle {
        height: parent.height
        anchors.right: parent.right
        anchors.top: parent.top
        implicitHeight: 32
        implicitWidth: 32 // Adjust width here
        color: sb.up.pressed ? Design.colButtonSelectedBackground : Design.colButtonHoveredBackground
        border.color: enabled ? Design.colButtonSelectedBackground : Design.colButtonHoveredBackground
        Text {
            text: '+'
            font.pointSize: 12
            anchors.centerIn: parent
        }
    }
    down.indicator: Rectangle {
        height: parent.height
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        implicitHeight: 30
        implicitWidth: 30 // Adjust width here
        color: sb.down.pressed ? Design.colButtonSelectedBackground : Design.colButtonHoveredBackground
        border.color: enabled ? Design.colButtonSelectedBackground : Design.colButtonHoveredBackground
        Text {
            text: '-'
            font.pointSize: 12
            anchors.centerIn: parent
        }
    }
}

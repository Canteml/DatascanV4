import QtQuick 2.15 //2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

import roboplanet.v2 1.0

Item {
    anchors.fill: parent;

    TableView {
        id: idMepTable
        anchors.fill: parent;
        anchors.topMargin: 35
        anchors.leftMargin: 100
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        contentHeight: rows * 35
        contentWidth: columns * 100

        interactive: false

        ScrollBar.vertical: ScrollBar {
            id: verticalScrollbar
            width: 15
            policy: ScrollBar.AlwaysOn
        }
        ScrollBar.horizontal: ScrollBar {
            id: horizontalScrollbar
            height: 15
            policy: ScrollBar.AlwaysOn
        }

        model: MepTableModel {}

        delegate: Rectangle {
            property int thisRow: row
            property int thisColumn: column
            implicitWidth: 100
            implicitHeight: 35
            border.width: 1

            color: ( (thisRow == examManager.mepRow) && (thisColumn == examManager.mepCol) ) ? "#FFE46A" : "white"

            Text {
                text: display
                anchors.centerIn: parent
                font.pointSize: 12
            }

            MouseArea {
                acceptedButtons: Qt.RightButton | Qt.LeftButton
                anchors.fill: parent
                onClicked: {
                    if(mouse.button === Qt.RightButton) {
                        examManager.removeMepAScan(parent.thisRow, parent.thisColumn)
                        return;
                    }
                    examManager.mepCol = parent.thisColumn
                    examManager.mepRow = parent.thisRow
                    examManager.isMepAutoMode = false
                }
            }
        }

    }

    HorizontalHeaderView {
        id: horizontalHeader
        clip: true
        syncView: idMepTable
        anchors.left: idMepTable.left
        height: 35
        delegate: Rectangle {
            implicitWidth: 100
            implicitHeight: 35
            border.width: 1
            color: examManager.mepCol === column ? "#243178" : "#333333"
            Text {
                text: display
                anchors.centerIn: parent
                font.pointSize: 10
                color: "white"
            }
        }
    }

    VerticalHeaderView {
        id: verticalHeader
        clip: true
        syncView: idMepTable
        anchors.top: idMepTable.top
        width: 100
        delegate: Rectangle {
            implicitWidth: 100
            implicitHeight: 35
            border.width: 1
            color: examManager.mepRow === row ? "#243178" : "#333333"
            Text {
                text: display
                anchors.centerIn: parent
                font.pointSize: 10
                color: "white"
            }
        }
    }

    /*Connections {
        target: examManager
        function onMepColChanged(col) {
            idMepTable.contentX = col
        }
    }

    Connections {
        target: examManager
        function onMepRowChanged(row) {

        }
    }*/

}

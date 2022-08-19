import QtQuick 2.15 //2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

import roboplanet.v2 1.0

Item {
    id: idMepView
    anchors.fill: parent;

    TableView {
        id: idMepTable
        anchors.fill: parent;

        property int cellWidth: 100 * examManager.mepZoom
        property int cellHeight: 35 * examManager.mepZoom
        property int scrollWith: 15 * examManager.mepZoom
        property int headerVWidth: 100 * examManager.mepZoom
        property int headerHHeight: 35 * examManager.mepZoom

        /*anchors.topMargin: 35 + ((contentHeight < height) ? (height-contentHeight) / 2.0 : 0);
        anchors.leftMargin: 100 + ((contentWidth < width) ? (width-contentWidth) / 2.0 : 0);*/
        anchors.topMargin: ((contentHeight + headerHHeight < parent.height) ? (headerHHeight + parent.height - contentHeight) / 2.0 : headerHHeight);
        anchors.leftMargin: ((contentWidth + headerVWidth < parent.width) ? (headerVWidth + parent.width - contentWidth) / 2.0 : headerVWidth);
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        contentHeight: rows * cellHeight
        contentWidth: columns * cellWidth

        interactive: false

        ScrollBar.vertical: ScrollBar {
            id: verticalScrollbar
            visible: (idMepTable.contentHeight + idMepTable.headerHHeight > idMepView.height)
            width: 15
            policy: ScrollBar.AlwaysOn
            contentItem: Rectangle {
                color: "#E5E5E5"
                radius: 30
                border.width: 1
            }
        }
        ScrollBar.horizontal: ScrollBar {
            id: horizontalScrollbar
            visible: (idMepTable.contentWidth + idMepTable.headerVWidth > idMepView.width)
            height: 15
            policy: ScrollBar.AlwaysOn
            contentItem: Rectangle {
                color: "#E5E5E5"
                radius: 30
                border.width: 1
            }
        }

        model: MepTableModel {}

        delegate: Rectangle {
            property int thisRow: row
            property int thisColumn: column
            implicitWidth: idMepTable.cellWidth
            implicitHeight: idMepTable.cellHeight
            border.width: 1

            color: ( (thisRow == examManager.mepRow) && (thisColumn == examManager.mepCol) ) ? "#FFE46A" : "white"

            Text {
                text: display
                anchors.centerIn: parent
                font.pointSize: 12 * examManager.mepZoom
            }

            MouseArea {
                acceptedButtons: Qt.RightButton | Qt.LeftButton
                anchors.fill: parent
                onPressed: {
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
        anchors.bottom: idMepTable.top
        height: idMepTable.headerHHeight
        delegate: Rectangle {
            implicitWidth: idMepTable.cellWidth
            implicitHeight: idMepTable.headerHHeight
            border.width: 1
            color: examManager.mepCol === column ? "#243178" : "#333333"
            Text {
                text: display
                anchors.centerIn: parent
                font.pointSize: 10 * examManager.mepZoom
                color: "white"
            }
        }
    }

    VerticalHeaderView {
        id: verticalHeader
        clip: true
        syncView: idMepTable
        anchors.top: idMepTable.top
        anchors.right: idMepTable.left
        width: idMepTable.headerVWidth
        delegate: Rectangle {
            implicitWidth: idMepTable.headerVWidth
            implicitHeight: idMepTable.cellHeight
            border.width: 1
            color: examManager.mepRow === row ? "#243178" : "#333333"
            Text {
                text: display
                anchors.centerIn: parent
                font.pointSize: 10 * examManager.mepZoom
                color: "white"
            }
        }
    }

    Connections {
        target: examManager
        function onMepColChanged(col) {
            if(idMepTable.contentWidth < idMepTable.width) return
            var currentX = idMepTable.contentX
            var leftX = col * idMepTable.cellWidth
            var rightX = (col + 1) * idMepTable.cellWidth
            if(leftX < currentX) {
                idMepTable.contentX = leftX
            } else if(rightX > currentX + idMepTable.width) {
                idMepTable.contentX = rightX - idMepTable.width
            }
        }
    }

    Connections {
        target: examManager
        function onMepRowChanged(row) {
            if(idMepTable.contentHeight < idMepTable.height) return
            var currentY = idMepTable.contentY
            var topY = row * idMepTable.cellHeight
            var bottomY = (row + 1) * idMepTable.cellHeight
            if(topY < currentY) {
                idMepTable.contentY = topY
            } else if(bottomY > currentY + idMepTable.height) {
                idMepTable.contentY = bottomY - idMepTable.height
            }
        }
    }

}

import QtQuick 2.15 //2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

import roboplanet.v2 1.0

Item {
    anchors.fill: parent;

    TableView {
        id: idMepTable
        anchors.fill: parent;
        anchors.topMargin: 30
        anchors.leftMargin: 30
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        ScrollBar.vertical: ScrollBar { id: verticalScrollbar ; width: 15 }
        ScrollBar.horizontal: ScrollBar { id: horizontalScrollbar ; height: 15 }

        model: MepTableModel {}

        delegate: Rectangle {
            property int thisRow: row
            property int thisColumn: column
            implicitWidth: 100
            implicitHeight: 50
            border.width: 1

            Text {
                text: display
                anchors.centerIn: parent
                font.pointSize: 10
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log(parent.thisRow)
                    console.log(parent.thisColumn)
                }
            }
        }

    }

    HorizontalHeaderView {
        id: horizontalHeader
        clip: true
        syncView: idMepTable
        anchors.left: idMepTable.left
        height: 30
        delegate: Rectangle {
            implicitWidth: 100
            implicitHeight: 30
            border.width: 1
            color: "#333333"
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
        width: 30
        delegate: Rectangle {
            implicitWidth: 30
            implicitHeight: 50
            border.width: 1
            color: "#333333"
            Text {
                text: display
                anchors.centerIn: parent
                font.pointSize: 10
                color: "white"
            }
        }
    }

}

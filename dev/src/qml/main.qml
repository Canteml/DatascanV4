import QtQuick 2.7
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2

Window  {
    objectName: "mainWindow"
    visible: true
    width: 640
    height: 480
    title: qsTr("DATASCAN")

    flags: Qt.Window

    // Trick to get focus wherever the mouse click, and remove focus from last item
    MouseArea {
        anchors.fill: parent
        onClicked: focus = true
    }

    Image {
        id: image1
        height: 270
        anchors.top: parent.top
        anchors.topMargin: 50
        anchors.right: parent.right
        anchors.rightMargin: 100
        anchors.left: parent.left
        anchors.leftMargin: 100
        source: "qrc:///img/logo.png"
    }

    Mainwindow {

    }
	

}

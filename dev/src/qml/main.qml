import QtQuick 2.7
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3


ApplicationWindow  {
    objectName: "mainWindow"
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
	
	Image {
		id: image1
		height: 270
		anchors.top: parent.top
		anchors.topMargin: 50
		anchors.right: parent.right
		anchors.rightMargin: 100
		anchors.left: parent.left
		anchors.leftMargin: 100
		source: "logo.png"
    }
}

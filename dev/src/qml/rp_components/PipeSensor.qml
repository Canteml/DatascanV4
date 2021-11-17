import QtQuick 2.15
import QtMultimedia 5.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0


Item {
    id: idRectPipeSensor
    Layout.fillWidth: true;
    Layout.fillHeight: true;

    property var sensorNumber;
    property alias sensorNumber : idRectPipeSensor.sensorNumber;

    property var selectShorcut1;
    property alias selectShorcut1 : idRectPipeSensor.selectShorcut1;
    property var selectShorcut2;
    property alias selectShorcut2 : idRectPipeSensor.selectShorcut2;

//    border.color: examPipeManager.currentSensorNumber === sensorNumber ? Design.colOrange : "white"
//    border.width: 2

    ColumnLayout {
        id: idColPipeSensor
        anchors.fill: parent
        spacing: 0

        Button {
            id: btTitle
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            text: qsTr(sensorNumber*90 + " °")

            checked: examPipeManager.currentSensorNumber === sensorNumber
            checkable: true

            background: Rectangle {
                color: (btTitle.pressed) ? Design.colButtonSelectedBackground : (btTitle.checked) ?  Design.colOrange : btTitle.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                border.width: examPipeManager.currentSensorNumber === sensorNumber ? 0 : 1
                border.color: Design.colFrameBorder
            }
            font.pointSize: 12

            Action {
                shortcut: selectShorcut1
                onTriggered: btTitle.keyAction()
            }

            Action {
                shortcut: selectShorcut2
                onTriggered: btTitle.keyAction()
            }

            onReleased:
            {
                keyAction()
            }

            function keyAction() {
                if( examPipeManager.currentSensorNumber !== sensorNumber ) {
                    examPipeManager.currentSensorNumber = sensorNumber
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            border.width: 1
            border.color: Design.colFrameBorder

            Text {
                id: idTxtThikness
                anchors.fill: parent
                font.pointSize: 12
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Image {
            id: idImgPicture
            Layout.fillWidth: true
            Layout.fillHeight : true;
            fillMode: Image.PreserveAspectFit
        }

        Connections {
            target : examPipeManager
            function onEvSensorThickessChanged(p_sensorNumber, p_sensorThickness) {
                if( p_sensorNumber === sensorNumber ) {
                    idTxtThikness.text = p_sensorThickness;
                }
            }
        }

        Connections {
            target : examPipeManager
            function onEvSensorPictureChanged(p_sensorNumber, p_sensorPictureFile) {
                if( p_sensorNumber === sensorNumber ) {
                    console.log(p_sensorPictureFile)
                    idImgPicture.source = "file:/"+p_sensorPictureFile;
                    //idImgPicture.source = "file:///C://Users//TLE-RP//Documents//Acq 0 - Capteur 1 2021-11-17 18-30-00.502.jpg"


                    // idImgPicture.source = "file:/C:\\Users\\TLE-RP\\Documents\\1.jpg" ///"file:///"+p_sensorPictureFile;
                }
            }
        }

        Connections {
            target : examPipeManager
            function onEvClearCircular() {
                idTxtThikness.text = "";
                idImgPicture.source = "";
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onReleased: {
            if( examPipeManager.currentSensorNumber !== sensorNumber ) {
                examPipeManager.currentSensorNumber = sensorNumber
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        border.color: examPipeManager.currentSensorNumber === sensorNumber ? Design.colOrange : "black"
        border.width: examPipeManager.currentSensorNumber === sensorNumber ? 3 : 1
        color: "transparent"
    }
}

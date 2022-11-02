import QtQuick 2.15
import QtMultimedia 5.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0

import VLCQt 1.1

Rectangle {
    id: idRectTopCam

    Rectangle {
        border.color: Design.colButtonBorder
        //color: "transparent"
        color: Design.colBackgroundDark
        anchors.fill: parent
    }

    // -- Camera
    Camera {
        id: idCameraMain
        objectName: "cameraMain"

        Component.onCompleted: {
            idCameraMain.stop()
        }

        imageCapture {
            onImageSaved : {
                cameraManager.signalMainCameraCaptureSaved(path)
            }
        }
    }

    Connections {
        target : cameraManager
        function onMainCameraIndexChanged() {
            if( cameraManager.getMainCameraIndex() !== -1 ) {
                idMainVideo.visible = true;
                idCameraMain.deviceId = cameraManager.getMainCameraId()
                if( globalManager.guiPage !== 1 && !cameraManager.isMainCameraIp ) { // page Config
                    idCameraMain.start()
                }
            } else {
                idMainVideo.visible = false;
            }
        }
    }

    Connections {
        target : cameraManager
        function onEvMainCameraReady() {
            if( cameraManager.getMainCameraIndex() !== -1  ) {
                idMainVideo.visible = true;
                idCameraMain.deviceId = cameraManager.getMainCameraId()
                if( globalManager.guiPage !== 1 && !cameraManager.isMainCameraIp ) { // page Config
                    idCameraMain.start()
                }
            } else {
                idMainVideo.visible = false;
            }
        }
    }

    Connections {
        target : cameraManager
        function onEvRequestMainCameraCapture(p_fileName) {
            idCameraMain.imageCapture.captureToLocation(p_fileName)
        }
    }

    Rectangle {
        id: idMainVideo
        anchors.margins: 1
        anchors.fill: parent
        visible: false
        VideoOutput {
            visible: !cameraManager.isMainCameraIp
            source: idCameraMain
            fillMode: VideoOutput.PreserveAspectCrop
            anchors.fill: parent
            focus : visible // to receive focus and capture key events when visible
        }
        VlcVideoOutput {
            visible : cameraManager.isMainCameraIp
            source: cameraManager.getMainCameraSource()
            anchors.fill: parent
            focus : visible // to receive focus and capture key events when visible
        }
    }

    ComboBox {
        id: idCbSelectCameraMain
        model: modelCameraRoleList
        visible: !cameraManager.selectMainCameraAuto
        anchors.horizontalCenter: idRectTopCam.horizontalCenter
        anchors.top : idRectTopCam.top
        anchors.topMargin: 0    // 5 ?
        width : 150
        height : 35
        font.pixelSize: 18
        property var ready : false

        background: Rectangle {
                color:"#FFFFFF"
                opacity: 0.6
                border.color: "#000000"
                border.width: 1
        }

        delegate : ItemDelegate {
            id: idItemDelegate
            width: idCbSelectCameraMain.width
            contentItem: Text {
                id:textItem
                text: modelData
                color: "#000000"
                font.pixelSize: 18

                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
            }

            background: Rectangle {
                color:idItemDelegate.hovered? Design.colButtonSelectedBackground : "white";
                anchors.left: idItemDelegate.left
                anchors.leftMargin: 0
                width:idItemDelegate.width
            }
        }

        onCurrentIndexChanged: {
            if( ready ) {
                cameraManager.setProductRoleMainCamera(idCbSelectCameraMain.currentIndex)
            }
        }

        Connections {
            target : cameraManager
            function onMainCameraIndexChanged() {
                if( idCbSelectCameraMain.currentIndex !== cameraManager.getProductRoleMainCamera() ) {
                    idCbSelectCameraMain.currentIndex = cameraManager.getProductRoleMainCamera();
                }
            }
        }

        Component.onCompleted: {
            idCbSelectCameraMain.currentIndex = cameraManager.getProductRoleMainCamera();
            ready = true;
        }
    }

    Button {
        id: idBtCapture
        visible: !cameraManager.selectMainCameraAuto
        anchors.right: idRectTopCam.right
        anchors.bottom: idRectTopCam.bottom
        anchors.margins: 5
        width : 100
        height : 35
        font.pixelSize: 18
        background: Rectangle {
            color: (idBtCapture.pressed) ? Design.colButtonSelectedBackground : idBtCapture.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
            opacity: 0.6
            border.width: 1
            border.color: Design.colFrameBorder
        }

        text: qsTr("Photo")
        font.pointSize: 18

        onPressed: {
            keyAction()
        }

        Action {
            shortcut: "c"
            enabled: examManager.controlType !== 2
            onTriggered: idBtCapture.keyAction()
        }

        Action {
            shortcut: "C"
            enabled: examManager.controlType !== 2
            onTriggered: idBtCapture.keyAction()
        }

        function keyAction() {
            cameraManager.captureMainCamera();
        }

        // Select camera (Control ASCAN , MEP, not for PANDA)
        Action {
            shortcut: "1"
            enabled: examManager.controlType !== 2
            onTriggered: cameraManager.setProductRoleMainCameraNoAlarm(0)
        }

        Action {
            shortcut: "2"
            enabled: examManager.controlType !== 2
            onTriggered: cameraManager.setProductRoleMainCameraNoAlarm(1)
        }

        Action {
            shortcut: "3"
            enabled: examManager.controlType !== 2
            onTriggered: cameraManager.setProductRoleMainCameraNoAlarm(2)
        }

        Action {
            shortcut: "4"
            enabled: examManager.controlType !== 2
            onTriggered: cameraManager.setProductRoleMainCameraNoAlarm(3)
        }
    }
}

import QtQuick 2.15
import QtMultimedia 5.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0

Rectangle {
    id: idRectTopCam

    Rectangle {
        border.color: "black"
        color: "transparent"
        anchors.fill: parent
    }

    // -- Camera
    Camera {
        id: idCameraMain
        objectName: "cameraMain"

        Component.onCompleted: {
            idCameraMain.deviceId = cameraManager.getMainCameraId();
            idCameraMain.stop()
        }
    }

    Connections {
        target : cameraManager
        function onMainCameraIndexChanged() {
            idCameraMain.deviceId = cameraManager.getMainCameraId();
        }
    }

    Connections {
        target : cameraManager
        function onEvMainCameraReady() {
            idCameraMain.deviceId = cameraManager.getMainCameraId();
        }
    }

    Connections {
        target : cameraManager
        function onEvRequestMainCameraCapture(p_fileName) {
            idCameraMain.imageCapture.captureToLocation(p_fileName)
            //idCameraMain.imageCapture.capture(p_fileName);
        }
    }

    VideoOutput {
        source: idCameraMain
        anchors.margins: 1
        anchors.fill: parent
        focus : visible // to receive focus and capture key events when visible
    }

    ComboBox {
        id: idCbSelectCameraMain
        model: modelCameraRoleList
        visible: !cameraManager.selectMainCameraAuto
        anchors.horizontalCenter: idRectTopCam.horizontalCenter
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
        anchors.margins: 10
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
            cameraManager.captureMainCamera();
        }
    }
}

import QtQuick 2.15
import QtMultimedia
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0
import Fonts 1.0

Rectangle {
    id: idRectTopCam

    Rectangle {
        border.color: Design.colButtonBorder
        color: Design.colBackgroundDark
        anchors.fill: parent
    }

    // -- Camera

    Connections {
        target: cameraManager
        function onMainCameraIndexChanged() {
            idMainVideo.visible = (cameraManager.getMainCameraIndex() !== -1)
        }
    }

    Connections {
        target: cameraManager
        function onEvMainCameraReady() {
            idMainVideo.visible = (cameraManager.getMainCameraIndex() !== -1)
        }
    }

    Rectangle {
        id: idMainVideo
        anchors.margins: 1
        anchors.fill: parent
        visible: false

        VideoOutput {
            id: idLocalVideo
            objectName: "mainLocalVideoOutput"
            visible: !cameraManager.isMainCameraIp
            fillMode: VideoOutput.PreserveAspectCrop
            anchors.fill: parent
            focus: visible
            Component.onCompleted: {
                cameraManager.connectMainSink(idLocalVideo)
            }
        }

        VideoOutput {
            id: idIpVideo
            objectName: "mainIpVideoOutput"
            visible: cameraManager.isMainCameraIp
            anchors.fill: parent
            focus: visible
            Component.onCompleted: {
                cameraManager.connectMainIpSink(idIpVideo)
            }
            Connections {
                target: cameraManager
                function onIsMainCameraIpChanged(isIp) {
                    if (isIp) cameraManager.connectMainIpSink(idIpVideo)
                }
            }
        }
    }

    ComboBox {
        id: idCbSelectCameraMain
        model: modelCameraRoleList
        visible: !cameraManager.selectMainCameraAuto
        anchors.right: idRectTopCam.right
        anchors.rightMargin: 5
        anchors.top: idRectTopCam.top
        anchors.topMargin: 5
        width: 150
        height: 35
        font.pixelSize: 18
        property var ready: false

        background: Rectangle {
            color: "#2C3B43"
            border.color: "#46606E"
            border.width: 2
        }

        contentItem: Text {
            leftPadding: 0
            rightPadding: idCbSelectCameraMain.indicator.width + idCbSelectCameraMain.spacing
            text: idCbSelectCameraMain.displayText
            font.bold: true
            font.family: Fonts.robotoCondensed
            font.pixelSize: 16
            color: "white"
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

        delegate: ItemDelegate {
            id: idItemDelegate
            width: idCbSelectCameraMain.width
            contentItem: Text {
                text: modelData
                color: idItemDelegate.hovered ? "white" : Design.colMainInk
                font.family: Fonts.robotoCondensed
                font.pixelSize: 18
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
            background: Rectangle {
                color: idItemDelegate.hovered ? Design.colButtonSelectedBackground : "white"
                anchors.left: idItemDelegate.left
                anchors.leftMargin: 0
                width: idItemDelegate.width
            }
        }

        onCurrentIndexChanged: {
            if (ready) {
                cameraManager.setProductRoleMainCamera(idCbSelectCameraMain.currentIndex)
            }
        }

        Connections {
            target: cameraManager
            function onMainCameraIndexChanged() {
                if (idCbSelectCameraMain.currentIndex !== cameraManager.getProductRoleMainCamera()) {
                    idCbSelectCameraMain.currentIndex = cameraManager.getProductRoleMainCamera()
                }
            }
        }

        Component.onCompleted: {
            idCbSelectCameraMain.currentIndex = cameraManager.getProductRoleMainCamera()
            ready = true
        }
    }

    Button {
        id: idBtCapture
        visible: !cameraManager.selectMainCameraAuto
        anchors.right: idRectTopCam.right
        anchors.bottom: idRectTopCam.bottom
        anchors.margins: 5
        width: 100
        height: 35
        font.pixelSize: 18
        background: Rectangle {
            color: "white"
            border.width: 2
            border.color: Design.colFrameBorder
        }
        contentItem: Text {
            text: qsTr("CAPTURE")
            color: "Black"
            font.bold: true
            font.family: Fonts.robotoCondensed
            font.pixelSize: 16
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
        onPressed: { keyAction() }

        Action { shortcut: "c"; enabled: examManager.controlType !== 2; onTriggered: idBtCapture.keyAction() }
        Action { shortcut: "C"; enabled: examManager.controlType !== 2; onTriggered: idBtCapture.keyAction() }

        function keyAction() { cameraManager.captureMainCamera() }

        Action { shortcut: "1"; enabled: examManager.controlType !== 2; onTriggered: cameraManager.setProductRoleMainCameraNoAlarm(0) }
        Action { shortcut: "2"; enabled: examManager.controlType !== 2; onTriggered: cameraManager.setProductRoleMainCameraNoAlarm(1) }
        Action { shortcut: "3"; enabled: examManager.controlType !== 2; onTriggered: cameraManager.setProductRoleMainCameraNoAlarm(2) }
        Action { shortcut: "4"; enabled: examManager.controlType !== 2; onTriggered: cameraManager.setProductRoleMainCameraNoAlarm(3) }
    }
}
    //////////////////////////////////////////////////////////////////////
/*
    Rectangle {
        id: idMainVideo
        anchors.margins: 1
        anchors.fill: parent
        visible: false

        // Camera locale USB - Qt6
        VideoOutput {
            id: idLocalVideo
            objectName: "mainLocalVideoOutput"
            visible: !cameraManager.isMainCameraIp
            fillMode: VideoOutput.PreserveAspectCrop
            anchors.fill: parent
            focus: visible

            Component.onCompleted: {
                cameraManager.connectMainSink(idLocalVideo)
            }
        }

        // Camera IP - Qt6
        VideoOutput {
            id: idIpVideo
            objectName: "mainIpVideoOutput"
            visible: cameraManager.isMainCameraIp
            anchors.fill: parent
            focus: visible

            Component.onCompleted: {
                cameraManager.connectMainIpSink(idIpVideo)
            }

            Connections {
                target: cameraManager
                function onIsMainCameraIpChanged(isIp) {
                    if (isIp) cameraManager.connectMainIpSink(idIpVideo)
                }
            }
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

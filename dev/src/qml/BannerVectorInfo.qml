import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import QtQuick.Dialogs 1.3 as Std
import Qt.labs.platform 1.1 as Platform
import Design 1.0
import QtMultimedia 5.15

Rectangle {
    id: idBannerVectorInfo
    Layout.fillHeight: true
    Layout.fillWidth: true

    border.width: 1
    border.color: "black"

    property alias idBtPositionPlus: btPositionPlus
    property alias idBtPositionMinus: btPositionMinus
    property alias idMepBtPositionPlus: mepBtPositionPlus
    property alias idMepBtPositionMinus: mepBtPositionMinus
    property alias idLePosition: lePosition
    property alias idMepLePosition: mepLePosition
    property alias btVectorConnection: btVectorConnection

    RowLayout {
        id: rowBannerVectorInfo
        anchors.fill: parent

        // Stack function to Exam control type
        StackLayout {
            id: idSlAcqMode
            currentIndex: examManager.controlType
            Layout.fillHeight: true
            Layout.fillWidth: true

            // --------------------------------------------------------------
            // PAGE ASCAN
            RowLayout {
                id: rowAScan
                Layout.fillHeight: true
                Layout.fillWidth: true

                Button {
                    id: btBannerTitle
                    text: examManager.currentExamName
                    font.pointSize: 12
                    Layout.preferredWidth: 160
                    Layout.fillWidth: false
                    Layout.fillHeight: true
                    padding: 0

                    contentItem: Text {
                        text: btBannerTitle.text
                        font: btBannerTitle.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        lineHeight: 1.25 // line-spacing
                    }

                    background: Rectangle {
                        id: ibBtBannerTitleBG
                        color: examManager.examStep !== 0 /*stopped*/ ?
                                   Design.colGreen : Design.colButtonBackground //btBannerTitle.pressed ? Design.colButtonSelectedBackground : btBannerTitle.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        //color: btBannerTitle.hovered ? "#9C9B9B" : "#FFFFFF"
                        border.width: 1
                        border.color: Design.colFrameBorder

    //                    ColorAnimation on color{
    //                        from:"#FFFFFFFF"
    //                        to:"#F0FF0000"
    //                        duration: 500
    //                        running:  examManager.examStep === 1 // recording measure
    //                        loops : Animation.Infinite
    //                        onRunningChanged: if(!running){ibBtBannerTitleBG.color = "#FFFFFFFF"} // reset
    //                    }
                    }

                    onPressed: { globalManager.guiPage = globalManager.guiPage === 3 ? 2 : 3;}
                }

                Button {
                    id: btScanStartPause

                    Layout.preferredWidth: idBannerVectorInfo.height - 10
                    Layout.preferredHeight: idBannerVectorInfo.height - 10

                    icon.source: examManager.examStep === 0 ? "qrc:///img/icons/btStart.png" :
                                 examManager.examStep === 1 ? "qrc:///img/icons/btPause.png" :
                                 examManager.examStep === 2 ? "qrc:///img/icons/btPause.png" :
                                 "qrc:///img/icons/iconUndo.png"

                    icon.width: idBannerVectorInfo.height - 10
                    icon.height: idBannerVectorInfo.height - 10
                    icon.color: "transparent"

                    antialiasing: true

                    background: Rectangle {
                        id: idBtStartBG
                        color: btScanStartPause.pressed ? Design.colButtonSelectedBackground : btScanStartPause.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        border.width: 1
                        border.color: Design.colButtonBorder
                        radius: 10

                        ColorAnimation on color{
                            from:"#FFFFFFFF"
                            to:"#F0FF0000"
                            duration: 500
                            running:  examManager.examStep === 2 // EXAM_PAUSED
                            loops : Animation.Infinite
                            onRunningChanged: if(!running){idBtStartBG.color = "#FFFFFFFF"} // reset
                        }
                    }

                    Action {
                        id: idActMin
                        onTriggered: btScanStartPause.keyAction()
                    }
                    Action {
                        id: idActMaj
                        onTriggered: btScanStartPause.keyAction()
                    }

                    onPressed: {
                        keyAction()
                    }

                    function keyAction() {
                        if( examManager.examStep === 0 ) {
                            examManager.examStep = 1    // record measure
                        } else if( examManager.examStep === 1 ) {
                            examManager.examStep = 2 // pause
                        } else if( examManager.examStep === 2 ) {
                            examManager.examStep = 1 // record measure
                        } else {
                            examManager.examStep = 2 // pause
                        }
                    }

                    Component.onCompleted: {
                        if( globalManager.keyboardQwerty ) {
                            idActMin.shortcut = "q";
                            idActMaj.shortcut = "Q";
                        } else {
                            idActMin.shortcut = "a";
                            idActMaj.shortcut = "A";
                        }
                    }

                    ToolTip.delay: 500
                    ToolTip.timeout: 5000
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Démarrer ou mettre en pause une acquisition.");
                }

                Button {
                    id: btReport

                    enabled: examManager.examStep === 1 || examManager.examStep === 2

                    Layout.preferredWidth: idBannerVectorInfo.height - 10
                    Layout.preferredHeight: idBannerVectorInfo.height - 10

                    icon.source: enabled ? "qrc:///img/icons/iconOk.png" : "qrc:///img/icons/iconOkDisabledBright.png"
                    icon.width: idBannerVectorInfo.height - 10
                    icon.height: idBannerVectorInfo.height - 10
                    icon.color: "transparent"

                    antialiasing: true

                    background: Rectangle {
                        id: btScanPauseBG
                        color: btReport.pressed ? Design.colButtonSelectedBackground : btReport.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        border.width: 1
                        border.color: Design.colButtonBorder
                        radius: 10
                    }

                    onPressed: {
                        examManager.examStep = 3    // export
                    }

                    ToolTip.delay: 500
                    ToolTip.timeout: 5000
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Générer un rapport BSCAN.");
                }

                Button {
                    id: btVectorDirection

                    Layout.preferredWidth: idBannerVectorInfo.height
                    Layout.preferredHeight: idBannerVectorInfo.height

                    icon.source: currentVector.funcPosition().paramLVectorDirX === 0 ? "qrc:///img/icons/altiscan_up_up.png" : "qrc:///img/icons/altiscan_up_down.png"
                    icon.width: idBannerVectorInfo.height
                    icon.height: idBannerVectorInfo.height
                    icon.color: "transparent"
                    padding: 1

                    background: Rectangle {
                        color: "transparent"
                    }

                    onPressed: {
                        // switch direction
                        currentVector.funcPosition().paramLVectorDirX = 1 - currentVector.funcPosition().paramLVectorDirX;
                    }

                    ToolTip.delay: 500
                    ToolTip.timeout: 5000
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Direction du porteur, permet d'inverser son sens de progression.");
                }

                Rectangle {
                    // seaparator
                    width: 5
                    Layout.fillHeight: true
                    color: "#9C9B9B"
                    Layout.topMargin: 5
                    Layout.bottomMargin: 5
                }

                // -------------------
                // -- Position
        //        Image {
        //            id: icPosition
        //            Layout.fillHeight: true
        //            Layout.topMargin: 5
        //            Layout.bottomMargin: 5
        //            Layout.preferredWidth: 40
        //            source: "qrc:///img/icons/iconPosition.png"
        //            fillMode: Image.PreserveAspectFit
        //        }

                RowLayout {

                    //Layout.fillHeight: true
                    TextInput {
                        id: lePosition
                        Layout.minimumWidth: 120

                        //Layout.fillHeight: true
                        text: "12.34"
                        font.family: "Helvetica"
                        font.pixelSize: 48
                        horizontalAlignment: Text.AlignRight
                        color: "black"
                        maximumLength: 5
                        smooth: true
                        validator: RegExpValidator {
                            regExp: /^(?:[1-9]\d*|0)?(?:\.\d+)?$/
                        }

                        onAccepted: focus = false

                        onFocusChanged: {
                            if(!focus) {
                                currentVector.funcPosition().setParamLBscanPosMmXFromString(text);
                            }
                        }
                    }

                    Item {
                        id: itPosition
                        Layout.fillHeight: true
                        Layout.preferredWidth: 40

                        Label {
                            id: lbPositionDim
                            anchors.fill: itPosition
                            anchors.bottomMargin: 10
                            text: "m"
                            font.pixelSize: 36
                            verticalAlignment: Text.AlignBottom
                        }

                        ColumnLayout {
                            anchors.fill: itPosition
                            anchors.topMargin: 5
                            anchors.bottomMargin: 5
                            spacing: 0

                            visible: lePosition.focus | btPositionPlus.focus | btPositionMinus.focus

                            Button {
                                id: btPositionPlus
                                text: "+"
                                font.pixelSize: 24
                                //Layout.preferredWidth: lbPositionDim.width
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                background: Rectangle {
                                    color: btPositionPlus.pressed ? Design.colButtonSelectedBackground : btPositionPlus.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                                    border.width: 1
                                    border.color: "black"
                                }

                                onPressed: lePosition.focus = true

                                //on: focus = false
                            }
                            Button {
                                id: btPositionMinus
                                text: "-"
                                font.pixelSize: 24
                                //Layout.preferredWidth: lbPositionDim.width
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                background: Rectangle {
                                    color: btPositionMinus.pressed ? Design.colButtonSelectedBackground : btPositionMinus.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                                    border.width: 1
                                    border.color: "black"
                                }

                                onPressed: lePosition.focus = true
                            }
                        }
                    }
                }

                Rectangle {
                    // seaparator
                    visible: currentVector.funcPressureSensor0().isEnabled && currentVector.funcPressureSensor0().hasReceivedMessage
                    width: 5
                    Layout.fillHeight: true
                    color: "#9C9B9B"
                    Layout.topMargin: 5
                    Layout.bottomMargin: 5
                }

                ColumnLayout {
                    visible: currentVector.funcPressureSensor0().isEnabled && currentVector.funcPressureSensor0().hasReceivedMessage
                    Layout.fillHeight: true
                    Layout.leftMargin: 20
                    spacing: 0

                    Text {
                        text: currentVector.funcPressureSensor0().depthType
                        font.pointSize: 12
                    }

                    Text {
                        text: currentVector.funcPressureSensor0().depthStrValue
                        font.pointSize: 30
                    }
                }

                Rectangle {
                    // seaparator
                    visible: currentVector.funcInertialSensor().isEnabled && currentVector.funcInertialSensor().hasReceivedMessage
                    width: 5
                    Layout.fillHeight: true
                    color: "#9C9B9B"
                    Layout.topMargin: 5
                    Layout.bottomMargin: 5
                }

                RowLayout {
                    visible: currentVector.funcInertialSensor().isEnabled && currentVector.funcInertialSensor().hasReceivedMessage
                    Layout.fillHeight: true
                    Layout.leftMargin: 20
                    spacing: 0

                    Image {
                        visible: false
                        Layout.topMargin: 10
                        Layout.bottomMargin: 10
                        Layout.fillHeight: true
                        rotation: currentVector.funcInertialSensor().paramLDirection
                        source: "qrc:///img/icons/directionArrow.png"
                        fillMode: Image.PreserveAspectFit
                        antialiasing: true
                    }

                    Text {
                        visible: false
                        text: currentVector.funcInertialSensor().paramLDirection + "°"
                        font.pointSize: 30
                    }

                }

                Item {
                    id: itSpacerHorizontal
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                }

            }

            // --------------------------------------------------------------
            // PAGE MEP
            RowLayout {
                id: rowMep
                Layout.fillHeight: true
                Layout.fillWidth: true

                Button {
                    Layout.fillHeight: true
                    Layout.margins: 10
                    font.pointSize: 12
                    enabled: currentVector.funcAScan0().hasReceivedMessage && !examManager.isMepAutoMode
                    text: qsTr("Mesure manuelle")
                    onPressed: {
                        examManager.startWaitingForMepAScan()
                    }
                }

                Switch {
                    id: idMepAutoSwitch
                    Layout.fillHeight: true
                    enabled: currentVector.funcAScan0().hasReceivedMessage
                    Layout.margins: 5
                    checked: examManager.isMepAutoMode
                    onClicked: {
                        examManager.isMepAutoMode = !examManager.isMepAutoMode
                    }
                    font.pointSize: 12
                    text: qsTr("Mode automatique")
                    background: Rectangle {
                        visible: currentVector.funcAScan0().hasReceivedMessage
                        border.width: 3
                        border.color: idMepAutoSwitch.checked ? "green" : "red"
                        radius: 15
                        SequentialAnimation on opacity {
                            loops: Animation.Infinite
                            running: !idMepAutoSwitch.checked
                            PropertyAnimation { to: 0 ; duration: 500 }
                            PropertyAnimation { to: 1.0 ; duration: 500 }
                        }
                        SequentialAnimation on opacity {
                            loops: Animation.Infinite
                            running: idMepAutoSwitch.checked
                            PropertyAnimation { to: 0 ; duration: 2000 }
                            PropertyAnimation { to: 1.0 ; duration: 2000 }
                        }
                    }
                }

                Button {
                    id: btVectorMepDirection

                    Layout.preferredWidth: idBannerVectorInfo.height
                    Layout.preferredHeight: idBannerVectorInfo.height

                    icon.source: currentVector.funcPosition().paramLVectorDirX === 0 ? "qrc:///img/icons/altiscan_up_up.png" : "qrc:///img/icons/altiscan_up_down.png"
                    icon.width: idBannerVectorInfo.height
                    icon.height: idBannerVectorInfo.height
                    icon.color: "transparent"
                    padding: 1

                    background: Rectangle {
                        color: "transparent"
                    }

                    onPressed: {
                        var newDirection = 1 - currentVector.funcPosition().paramLVectorDirX;
                        currentVector.funcPosition().paramLVectorDirX = newDirection;
                        var maxRow = examManager.mepNbRow - 1;
                        if( (newDirection === 0) && (examManager.mepRow === 0) ) {
                            examManager.mepRow = examManager.mepNbRow - 1;
                        } else if( (newDirection === 1) && (examManager.mepRow === maxRow) ) {
                            examManager.mepRow = 0;
                        }
                    }

                    ToolTip.delay: 500
                    ToolTip.timeout: 5000
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Direction du porteur, permet d'inverser son sens de progression.");
                }

                Rectangle {
                    // separator
                    width: 5
                    Layout.fillHeight: true
                    color: "#9C9B9B"
                    Layout.topMargin: 5
                    Layout.bottomMargin: 5
                }

                RowLayout {

                    //Layout.fillHeight: true
                    TextInput {
                        id: mepLePosition
                        Layout.minimumWidth: 120

                        //Layout.fillHeight: true
                        text: "12.34"
                        font.family: "Helvetica"
                        font.pixelSize: 48
                        horizontalAlignment: Text.AlignRight
                        color: "black"
                        maximumLength: 5
                        smooth: true
                        validator: RegExpValidator {
                            regExp: /^(?:[1-9]\d*|0)?(?:\.\d+)?$/
                        }

                        onAccepted: focus = false

                        onFocusChanged: {
                            if(!focus) {
                                currentVector.funcPosition().setParamLBscanPosMmXFromString(text);
                            }
                        }
                    }

                    Item {
                        id: mepItPosition
                        Layout.fillHeight: true
                        Layout.preferredWidth: 40

                        Label {
                            id: mepLbPositionDim
                            anchors.fill: mepItPosition
                            anchors.bottomMargin: 10
                            text: "m"
                            font.pixelSize: 36
                            verticalAlignment: Text.AlignBottom
                        }

                        ColumnLayout {
                            anchors.fill: mepItPosition
                            anchors.topMargin: 5
                            anchors.bottomMargin: 5
                            spacing: 0

                            visible: mepLePosition.focus | mepBtPositionPlus.focus | mepBtPositionPlus.focus

                            Button {
                                id: mepBtPositionPlus
                                text: "+"
                                font.pixelSize: 24
                                //Layout.preferredWidth: lbPositionDim.width
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                background: Rectangle {
                                    color: mepBtPositionPlus.pressed ? Design.colButtonSelectedBackground : mepBtPositionPlus.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                                    border.width: 1
                                    border.color: "black"
                                }

                                onPressed: mepLePosition.focus = true

                                //on: focus = false
                            }
                            Button {
                                id: mepBtPositionMinus
                                text: "-"
                                font.pixelSize: 24
                                //Layout.preferredWidth: lbPositionDim.width
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                background: Rectangle {
                                    color: mepBtPositionMinus.pressed ? Design.colButtonSelectedBackground : mepBtPositionMinus.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                                    border.width: 1
                                    border.color: "black"
                                }

                                onPressed: mepLePosition.focus = true
                            }
                        }
                    }
                }

                Rectangle {
                    // separator
                    width: 5
                    Layout.fillHeight: true
                    color: "#9C9B9B"
                    Layout.topMargin: 5
                    Layout.bottomMargin: 5
                }

                Button {
                    id: idBtMepSave
                    antialiasing: true
                    Layout.fillHeight: true
                    Layout.margins: 10
                    Layout.preferredWidth: idBtMepSave.height
                    background: Rectangle {
                        color: idBtMepSave.pressed ? Design.colButtonSelectedBackground : idBtMepSave.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        border.width: 1
                        border.color: Design.colButtonBorder
                        radius: 3
                    }

                    icon.source: "qrc:///img/icons/fileSave.png"
                    icon.color: "transparent"
                    icon.height: 30
                    icon.width: 30
                    padding: 1

                    onPressed: {
                        idFdMepExport.visible = true;
                    }

                    ToolTip.delay: 1000
                    ToolTip.timeout: 5000
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Sauvegarder l'examen en cours.")
                }

                Platform.FileDialog  {
                    id: idFdMepExport
                    title: qsTr("Enregistrement de l'examen ...")
                    nameFilters: ["Excel (*.xlsx)"]
                    fileMode : Platform.FileDialog.SaveFile
                    property var retExport;

                    folder: "file:///" + examManager.exportFolder
                    onAccepted: {
                        retExport = mepExporter.startExport(currentFile)
                        if( retExport !== "" ) {
                            idMsgError.visible = true
                        }
                    }
                }

                Std.MessageDialog {
                    id: idMsgError
                    icon: Std.StandardIcon.Critical
                    title: qsTr("Erreur")
                    text: qsTr("L'examen n'a pas pu être sauvegardé :\n" + idFdMepExport.retExport )
                    standardButtons: Std.StandardButton.Ok
                }

            }

            // --------------------------------------------------------------
            // PAGE PIPE (Panda)
            RowLayout {
                id: rowPanda
                Layout.fillHeight: true
                Layout.fillWidth: true

                Button {
                    id: btBannerTitlePipe
                    text: qsTr("Acquisition\n#" + (examPipeManager.acquisitionNumber+1))
                    font.pointSize: 12
                    Layout.preferredWidth: 160
                    Layout.fillWidth: false
                    Layout.fillHeight: true
                    padding: 0

                    contentItem: Text {
                        text: btBannerTitlePipe.text
                        font: btBannerTitlePipe.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        lineHeight: 1.25 // line-spacing
                    }

                    background: Rectangle {
                        id: ibBtBannerTitlePipeBG
                        color: Design.colButtonBackground
                        border.width: 1
                        border.color: Design.colFrameBorder
                    }
                    onPressed: { globalManager.guiPage = 3;}
                }

                Image {
                    source: "qrc:///img/icons/iconNote.png"
                    Layout.preferredHeight: 60
                    Layout.preferredWidth: 60
                    Layout.fillWidth: false
                }

                Rectangle {
                    id: txtCommentBackground
                    Layout.topMargin: 5
                    Layout.bottomMargin: 5
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    color: Design.colButtonHoveredBackground

                    TextArea  {
                        id: txtComment
                        anchors.fill: parent
                        font.pointSize: 12

                        Keys.onReturnPressed: {
                            if(!event.modifiers) {

                                txtComment.focus = false
                            }
                            else {
                                // Ignore other cases
                                event.accepted = false;
                            }
                        }

                        onEditingFinished: {
                            examPipeManager.examCurrentComment = text
                        }

                        hoverEnabled: true

                        ToolTip.delay: 500
                        ToolTip.timeout: 5000
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Commentaire sur les acquisitions en cours.")
                    }

                }


                Connections {
                    target : examPipeManager
                    function onEvClearCircular() {
                        txtComment.text = "";
                    }
                }
            }
        }

        // -------------------
        // Camera

        Camera {
            id: idCameraAlarm
            objectName: "cameraAlarm"            

            Component.onCompleted: {
                idCameraAlarm.stop()
            }
        }

        Connections {
            target : cameraManager.camAlarm()
            function onCameraVideoChanged() {
                if( cameraManager.camAlarm().isEnabled && cameraManager.camAlarm().cameraVideo ) {
                    idCameraAlarm.deviceId = cameraManager.camAlarm().cameraVideo.getCameraId();
                    idFrameCamAlarm.visible = true
                    if( globalManager.guiPage !== 1 ) { // page Config
                        idCameraAlarm.start()
                    }
                } else {
                    idFrameCamAlarm.visible = false
                }
            }
        }

        Connections {
            target : cameraManager
            function onEvAlarmCameraReady() {
                if( cameraManager.camAlarm().isEnabled && cameraManager.camAlarm().cameraVideo ) {
                    idCameraAlarm.deviceId = cameraManager.camAlarm().cameraVideo.getCameraId();
                    idFrameCamAlarm.visible = true
                    if( globalManager.guiPage !== 1 ) { // page Config
                        idCameraAlarm.start()
                    }
                } else {
                    idFrameCamAlarm.visible = false
                }
            }
        }

        Rectangle {
            id: idFrameCamAlarm
            visible : cameraManager.camAlarm().isEnabled && cameraManager.camAlarm().cameraVideo
            border.color: Design.colFrameBorder
            border.width: 1
            Layout.preferredHeight: idVideo.height+2
            Layout.preferredWidth: idVideo.width+2
            VideoOutput {
                id: idVideo
                // Keept aspect ration of camera frame
                height: 60
                width: sourceRect.width * 60 / sourceRect.height

                // Frame around the camera
                x:1
                y:1

                source: idCameraAlarm
            }

            MouseArea {
                id: idMaCamera
                anchors.fill: parent

                onPressed: {
                    keyAction()
                }

                Action {
                    shortcut: "0"
                    onTriggered: idMaCamera.keyAction()
                }

                function keyAction() {
                    cameraManager.setRoleMainCamera(0);
                }
            }
        }

        Rectangle {
            visible: false //put to true to set visible the button

            // seaparator
            width: 5
            Layout.fillHeight: true
            color: Design.colSeparator
            Layout.topMargin: 5
            Layout.bottomMargin: 5
        }

        // -------------------
        // Peripherals
        Button {
            id: icJoystick

            visible: false //put to true to set visible the button

            Layout.fillHeight: true
            Layout.topMargin: 5
            Layout.bottomMargin: 5           

//            Layout.preferredWidth: idBannerVectorInfo.height
//            Layout.preferredHeight: idBannerVectorInfo.height

            icon.source: joystickManager.isConnected ? "qrc:///img/icons/iconJoystickConnected.png" : "qrc:///img/icons/iconJoystickDisconnected.png"
            icon.width: idBannerVectorInfo.height
            icon.height: idBannerVectorInfo.height
            icon.color: "transparent"
            padding: 1

            background: Rectangle {
                color: "transparent"
            }

            hoverEnabled: true

            ToolTip.delay: 500
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Etat de la connection du joystick : " + (joystickManager.isConnected ? qsTr("Connecté") : qsTr("Déconnecté")));
        }

        ColumnLayout {
            id: idLyBattery
            //visible: currentVector.funcBattery().paramDBatteryLvlP !== 255
            visible: false
            Layout.fillHeight: true
            spacing : 0
            Layout.topMargin: 5
            Layout.bottomMargin: 5

            RowLayout {
                Layout.fillHeight: true
                Item {
                    Layout.fillHeight: true
                    // Spacer
                }
                Image {
                    //Layout.topMargin: 5
                    //Layout.bottomMargin: 5
                    Layout.preferredWidth: 20
                    Layout.fillHeight: true
                    source: currentVector.funcBattery().paramDBatteryLvlP >= 90 ? "qrc:///img/icons/iconBat100.png" :
                            currentVector.funcBattery().paramDBatteryLvlP >= 75 ? "qrc:///img/icons/iconBat75.png" :
                            currentVector.funcBattery().paramDBatteryLvlP >= 50 ? "qrc:///img/icons/iconBat50.png" :
                            currentVector.funcBattery().paramDBatteryLvlP >= 25 ? "qrc:///img/icons/iconBat25.png" :
                            currentVector.funcBattery().paramDBatteryLvlP >= 10 ? "qrc:///img/icons/iconBat00.png" :
                            "qrc:///img/icons/iconBatEmpty.png"
                    fillMode: Image.PreserveAspectFit
                }

                Item {
                    Layout.fillHeight: true
                    // Spacer
                }
            }

            Text {
                text : currentVector.funcBattery().paramDBatteryLvlP + " %"
            }
        }

        Rectangle {
            visible: true //put to true to set visible the button

            // seaparator
            width: 5
            Layout.fillHeight: true
            color: Design.colSeparator
            Layout.topMargin: 5
            Layout.bottomMargin: 5
        }

        // -------------------
        // Connections
        Button {
            id: btVectorConnection

            Layout.preferredWidth: idBannerVectorInfo.height
            Layout.preferredHeight: idBannerVectorInfo.height

            icon.source: "qrc:///img/icons/iconDisconnected.png"
            icon.width: idBannerVectorInfo.height
            icon.height: idBannerVectorInfo.height
            icon.color: "transparent"
            padding: 1

            background: Rectangle {
                color: "transparent"
            }

            hoverEnabled: true

            ToolTip.delay: 500
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Etat de la connection avec le porteur : " + (2 === currentVector.FuncStateConnection ? qsTr("Connecté") : 1 === currentVector.FuncStateConnection ? qsTr("Partiellement connecté") : qsTr("Déconnecté")));

            Timer {
                id: btVectorConnectionTimer
                interval: 3000
                running: false
                repeat: false
            }
        }

        Button {
            id: btVectorEmergencyStop
            visible: false //put to true to set visible the button

            Layout.preferredWidth: idBannerVectorInfo.height
            Layout.preferredHeight: idBannerVectorInfo.height

            icon.source: "qrc:///img/icons/btEmergencyStop.png"
            icon.width: idBannerVectorInfo.height
            icon.height: idBannerVectorInfo.height
            icon.color: "transparent"
            padding: 2
            property var emergencyActivated : false

            background: Rectangle {
                id: rectBtVectorEmergencyStop
                color: (btVectorEmergencyStop.pressed || btVectorEmergencyStop.emergencyActivated) ? "#FF0000" : btVectorEmergencyStop.hovered ? Design.colOrange : "transparent"
                // keep 1 pixel clean arround the button to not override the layout frame
                width: btVectorEmergencyStop.width-2
                x : 1
                height: btVectorEmergencyStop.height-2
                y : 1
            }

            onPressed: {
                keyAction();
            }

            Action {
                shortcut: "Escape"
                onTriggered: {
                    btVectorEmergencyStop.emergencyActivated = true;
                    timDisEm.start()
                    btVectorEmergencyStop.keyAction()
                }
            }

            Timer {
                id: timDisEm
                interval: 500; running: false; repeat: false
                onTriggered:  btVectorEmergencyStop.emergencyActivated = false;
            }

            function keyAction() {
                currentVector.emergencyStop();
            }

            ToolTip.delay: 500
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Arrêt d'urgence");


        }
    }

    Connections {
        target : currentVector.funcPosition()
        function onParamLBscanPosMmXChanged() {
            updateLePosition();
        }
    }

    Connections {
        target : idBtPositionPlus
        function onPressed() {
            currentVector.funcPosition().bscanPosMmXAddMm(10);
        }
    }

    Connections {
        target : idBtPositionMinus
        function onPressed() {
            currentVector.funcPosition().bscanPosMmXAddMm(-10);
        }
    }

    Connections {
        target : idMepBtPositionPlus
        function onPressed() {
            currentVector.funcPosition().bscanPosMmXAddMm(10);
        }
    }

    Connections {
        target : idMepBtPositionMinus
        function onPressed() {
            currentVector.funcPosition().bscanPosMmXAddMm(-10);
        }
    }

    Connections {
        target : currentVector
        function onEvFuncStateConnectionChanged() {
            if( 2 === currentVector.FuncStateConnection ) {
                btVectorConnectionTimer.stop()
                btVectorConnection.icon.source = "qrc:///img/icons/iconConnected.png"
            } else {
                btVectorConnectionTimer.start()
            }
        }
    }

    Connections {
        target: btVectorConnectionTimer
        function onTriggered() {
            if( 2 === currentVector.FuncStateConnection ) {
                btVectorConnection.icon.source = "qrc:///img/icons/iconConnected.png"
            } else if( 1 === currentVector.FuncStateConnection ) {
                btVectorConnection.icon.source = "qrc:///img/icons/iconPartiallyConnected.png"
            } else {
                btVectorConnection.icon.source = "qrc:///img/icons/iconDisconnected.png"
            }
        }
    }

    Component.onCompleted: {
        updateLePosition();
    }

    function updateLePosition() {
        idLePosition.text = currentVector.funcPosition().getBScanPosMeterWith2Decimals();
        idMepLePosition.text = currentVector.funcPosition().getBScanPosMeterWith2Decimals();//(Math.round(currentVector.funcPosition().paramLBscanPosMmX/10)/100);
    }

}

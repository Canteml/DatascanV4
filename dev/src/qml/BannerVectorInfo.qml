import QtQuick
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import QtQuick.Dialogs as Std
import Qt.labs.platform 1.1 as Platform
import Design 1.0
import Fonts 1.0
import QtMultimedia

Rectangle {
    id: idBannerVectorInfo
    Layout.fillHeight: true
    Layout.fillWidth: true

    color: "transparent"
    border.width: 1
    border.color: "black"

    property alias idBtPositionPlus: btPositionPlus
    property alias idBtPositionMinus: btPositionMinus
    property alias idMepBtPositionPlus: mepBtPositionPlus
    property alias idMepBtPositionMinus: mepBtPositionMinus
    property alias idLePosition: lePosition
    property alias idMepLePosition: mepLePosition
    property alias btVectorConnection: btVectorConnection
    property alias btMainMenu: btMainMenu

    RowLayout {
        id: rowBannerVectorInfo
        anchors.fill: parent

        Button {
            id: btMainMenu
            Layout.preferredWidth: idBannerVectorInfo.height - 15
            Layout.preferredHeight: idBannerVectorInfo.height - 15
            Layout.leftMargin: 10
            Layout.fillWidth: false
            Layout.fillHeight: false

            highlighted: true

            icon.source: "qrc:///img/icons/btMainMenu.png"
            icon.width: idBannerVectorInfo.height
            icon.height: idBannerVectorInfo.height
            icon.color: "transparent"

            background: Rectangle {
                color: btMainMenu.pressed ? Design.colButtonSelectedBackground : btMainMenu.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                border.width: 3
                border.color: Design.colButtonBorder
            }

            padding: 0
        }

        // Stack function to Exam control type
        StackLayout {
            id: idSlAcqMode
            currentIndex: examManager.controlType
            Layout.fillHeight: true
            Layout.fillWidth: true
            // Only the visible page constrains the width, so the banner can shrink with the window
            Layout.minimumWidth: currentIndex === 0 ? rowAScan.implicitWidth
                               : currentIndex === 1 ? rowMep.implicitWidth
                               : rowPanda.implicitWidth

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
                        font.bold: true
                        font.pixelSize: 16
                        font.family: Fonts.robotoCondensed
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        lineHeight: 1.25 // line-spacing
                        color: "white"
                    }

                    background: Rectangle {
                        id: ibBtBannerTitleBG
                        color: examManager.examStep !== 0 /*stopped*/ ?
                                   Design.colGreen : "transparent" //btBannerTitle.pressed ? Design.colButtonSelectedBackground : btBannerTitle.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        //color: btBannerTitle.hovered ? "#9C9B9B" : "#FFFFFF"


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

                Rectangle {
                    // seaparator
                    width: 3
                    Layout.fillHeight: true
                    color: "#2F5E72"
                    Layout.topMargin: 5
                    Layout.bottomMargin: 5
                }

                Button {
                    id: btScanStartPause

                    Layout.preferredWidth: 70
                    Layout.preferredHeight: idBannerVectorInfo.height - 15

                    /*
                    icon.source: examManager.examStep === 0 ? "qrc:///img/icons/btStart.png" :
                                 examManager.examStep === 1 ? "qrc:///img/icons/btPause.png" :
                                 examManager.examStep === 2 ? "qrc:///img/icons/btPause.png" :
                                 "qrc:///img/icons/iconUndo.png"

                    icon.width: idBannerVectorInfo.height - 15
                    icon.height: idBannerVectorInfo.height - 15
                    icon.color: "transparent"

                    antialiasing: true
                    */

                    // PLAY when stopped or paused, PAUSE while recording
                    readonly property bool isRecording: examManager.examStep === 1

                    contentItem: Text {
                        text: btScanStartPause.isRecording ? "PAUSE" : "PLAY"
                        font.family: Fonts.robotoCondensed
                        font.pointSize: 12
                        font.bold: true
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        id: idBtStartBG
                        color: btScanStartPause.isRecording
                               ? (btScanStartPause.pressed ? Design.colOrangeSelected : Design.colOrange)
                               : (btScanStartPause.pressed ? Qt.darker(Design.colGreen, 1.3) : Design.colGreen)
                        border.width: 3
                        border.color: Design.colButtonBorder

                        // Blinks while the acquisition is paused
                        SequentialAnimation on opacity {
                            running: examManager.examStep === 2 // EXAM_PAUSED
                            loops: Animation.Infinite
                            PropertyAnimation { to: 0.3; duration: 500 }
                            PropertyAnimation { to: 1.0; duration: 500 }
                            onRunningChanged: if(!running){idBtStartBG.opacity = 1.0} // reset
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
                        if( examManager.examStep === 1 ) {
                            examManager.examStep = 2    // pause
                        } else if( examManager.examStep === 0 || examManager.examStep === 2 ) {
                            examManager.examStep = 1    // record measure (start or resume)
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
                    ToolTip.text: btScanStartPause.isRecording ? qsTr("Mettre en pause l'acquisition.")
                                : examManager.examStep === 2 ? qsTr("Reprendre l'acquisition.")
                                : qsTr("Démarrer une acquisition.");
                }

                Button {
                    id: btScanStop

                    enabled: examManager.examStep === 1 || examManager.examStep === 2
                    opacity: enabled ? 1.0 : 0.4

                    Layout.preferredWidth: 70
                    Layout.preferredHeight: idBannerVectorInfo.height - 15

                    /*
                    icon.source: enabled ? "qrc:///img/icons/iconOk.png" : "qrc:///img/icons/iconOkDisabledBright.png"
                    icon.width: idBannerVectorInfo.height - 10
                    icon.height: idBannerVectorInfo.height - 10
                    icon.color: "transparent"

                    antialiasing: true
                    */

                    contentItem: Text {
                        text: "STOP"
                        font.family: Fonts.robotoCondensed
                        font.pointSize: 12
                        font.bold: true
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        color: btScanStop.pressed ? Design.colButtonSelectedBackground : btScanStop.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        border.width: 3
                        border.color: Design.colButtonBorder
                    }

                    onPressed: {
                        examManager.examStep = 3    // stop : export then back to stopped
                    }

                    ToolTip.delay: 500
                    ToolTip.timeout: 5000
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Arrêter l'acquisition et générer le rapport BSCAN.");
                }

                Button {
                    id: btVectorDirection

                    Layout.preferredWidth: idBannerVectorInfo.height - 15
                    Layout.preferredHeight: idBannerVectorInfo.height - 15

                    /*
                    icon.source: currentVector.funcPosition().paramLVectorDirX === 0 ? "qrc:///img/icons/altiscan_up_up.png" : "qrc:///img/icons/altiscan_up_down.png"
                    icon.width: btVectorDirection.height
                    icon.height: btVectorDirection.height
                    icon.color: "transparent"
                    padding: 1
                    */

                    contentItem: Text {
                        text: qsTr("◉")
                        font.family: Fonts.robotoCondensed
                        font.pointSize: 40
                        font.bold: true
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        color: Design.colButtonBackground
                        border.width: 3
                        border.color: Design.colButtonBorder
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

                // Spacer: pushes the position next to the right-hand blocks and shrinks first when the window narrows
                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                RowLayout {
                    Layout.alignment: Qt.AlignVCenter
                    Layout.rightMargin: 5
                    spacing: 2

                    //Layout.fillHeight: true
                    TextInput {
                        id: lePosition

                        //Layout.fillHeight: true
                        text: "12.34"
                        font.family: Fonts.robotoCondensed
                        font.pixelSize: 36
                        horizontalAlignment: Text.AlignRight
                        color: "white"
                        maximumLength: 5
                        smooth: true
                        validator: RegularExpressionValidator {
                            regularExpression: /^(?:[1-9]\d*|0)?(?:\.\d+)?$/
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
                        Layout.preferredWidth: 28

                        Label {
                            id: lbPositionDim
                            // Same baseline as the position value
                            x: 0
                            y: lePosition.y - itPosition.y + lePosition.baselineOffset - baselineOffset
                            text: "m"
                            font.family: Fonts.robotoCondensed
                            font.pixelSize: 26
                            color: "#2F5E72"
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
                                    border.color: Design.colButtonBorder
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
                                    border.color: Design.colButtonBorder
                                }

                                onPressed: lePosition.focus = true
                            }
                        }
                    }
                }

                /*
                ColumnLayout {
                    visible: currentVector.funcPressureSensor0().isEnabled && currentVector.funcPressureSensor0().hasReceivedMessage
                    Layout.fillHeight: true
                    Layout.leftMargin: 20
                    spacing: 0

                    Text {
                        text: currentVector.funcPressureSensor0().depthType
                        font.family: Fonts.robotoCondensed
                        font.pointSize: 12
                    }

                    Text {
                        text: currentVector.funcPressureSensor0().depthStrValue
                        font.family: Fonts.robotoCondensed
                        font.pointSize: 30
                    }
                }

                Rectangle {
                    // seaparator
                    // visible: currentVector.funcInertialSensor().isEnabled && currentVector.funcInertialSensor().hasReceivedMessage
                    width: 5
                    Layout.fillHeight: true
                    color: "#9C9B9B"
                    Layout.topMargin: 5
                    Layout.bottomMargin: 5
                }

                RowLayout {
                    // visible: currentVector.funcInertialSensor().isEnabled && currentVector.funcInertialSensor().hasReceivedMessage
                    Layout.fillHeight: true
                    Layout.leftMargin: 20
                    spacing: 0

                    Image {
                        Layout.topMargin: 10
                        Layout.bottomMargin: 10
                        Layout.fillHeight: true
                        rotation: currentVector.funcInertialSensor().paramLDirection
                        source: "qrc:///img/icons/directionArrow.png"
                        fillMode: Image.PreserveAspectFit
                        antialiasing: true
                    }

                    Text {
                        text: currentVector.funcInertialSensor().paramLDirection + "°"
                        font.family: Fonts.robotoCondensed
                        font.pointSize: 30
                    }

                }

                Item {
                    id: itSpacerHorizontal
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                }*/

            }

            // --------------------------------------------------------------
            // PAGE MEP
            RowLayout {
                id: rowMep
                Layout.fillHeight: true
                Layout.fillWidth: true

                Button {
                    id: btMesureManuelle
                    text: qsTr("Mesure manuelle")
                    font.pointSize: 12
                    palette.buttonText: "white"
                    Layout.fillHeight: true
                    Layout.margins: 10
                    enabled: currentVector.funcAScan0().hasReceivedMessage && !examManager.isMepAutoMode
                    background: Rectangle{
                        color: btMesureManuelle.pressed ? Design.colButtonSelectedBackground : btMesureManuelle.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        border.width: 3
                        border.color: Design.colButtonBorder
                    }
                    onPressed: {
                        examManager.startWaitingForMepAScan()
                    }

                    icon.width: idBannerVectorInfo.height - 15
                    icon.height: idBannerVectorInfo.height - 15
                    icon.color: "transparent"

                    antialiasing: true
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
                    palette.buttonText: "white"
                    background: Rectangle {
                        visible: currentVector.funcAScan0().hasReceivedMessage
                        color: Design.colButtonSelectedBackground
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

                    Layout.preferredWidth: idBannerVectorInfo.height - 15
                    Layout.preferredHeight: idBannerVectorInfo.height - 15

                    icon.source: currentVector.funcPosition().paramLVectorDirX === 0 ? "qrc:///img/icons/altiscan_up_up.png" : "qrc:///img/icons/altiscan_up_down.png"
                    icon.width: idBannerVectorInfo.height - 15
                    icon.height: idBannerVectorInfo.height - 15
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
                    // seaparator
                    width: 3
                    Layout.fillHeight: true
                    color: "#2F5E72"
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
                        color: "white"
                        maximumLength: 5
                        smooth: true
                        validator: RegularExpressionValidator {
                            regularExpression: /^(?:[1-9]\d*|0)?(?:\.\d+)?$/
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
                            color: "#2F5E72"
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
                    // seaparator
                    width: 3
                    Layout.fillHeight: true
                    color: "#2F5E72"
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
                    title: qsTr("Erreur")
                    text: qsTr("L'examen n'a pas pu être sauvegardé :\n" + idFdMepExport.retExport)
                    buttons: Std.MessageDialog.Ok
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
                        font.family: Fonts.robotoCondensed
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

                // source: idCameraAlarm
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
            visible: currentVector.funcBattery().paramDBatteryLvlP !== 255
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
                font.family: Fonts.robotoCondensed
            }
        }
        Rectangle {
            id: rectangleEnregistre
            Layout.preferredWidth: 170
            Layout.preferredHeight: idBannerVectorInfo.height - 15
            Layout.alignment: Qt.AlignVCenter
            antialiasing: true

            border.width: 3
            border.color: Design.colButtonBorder

            color: Design.colButtonBackground

            Text {
                // "MODE LIBRE" at startup and in free session, "ENREGISTRÉ" once a planner is loaded
                text: (examManager.importMessages.length > 0 && !examManager.freeSession)
                      ? qsTr("ENREGISTRÉ") : qsTr("MODE LIBRE")
                font.family: Fonts.robotoCondensed
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
                font.bold: true
                font.pixelSize: 16
            }
        }

        Rectangle {

            Layout.preferredWidth: 170
            Layout.preferredHeight: idBannerVectorInfo.height - 15
            Layout.alignment: Qt.AlignVCenter
            antialiasing: true

            readonly property bool plannerLoaded: examManager.importMessages.length > 0
            // Free session is always compliant; otherwise it depends on the planner import
            readonly property bool conforme: examManager.freeSession || examManager.importConforme
            // Startup: neither free session nor planner -> not traced
            readonly property bool nonTracee: !examManager.freeSession && !plannerLoaded

            border.width: 3
            border.color: "white"

            // Red when not traced, green when compliant, orange on any import warning/error
            color: nonTracee ? "#C62828"
                 : conforme ? "#12603F" : "#E07B00"
            Text {
                text: parent.nonTracee ? qsTr("NON TRACÉE")
                    : parent.conforme ? qsTr("CONFORME") : qsTr("NON CONFORME")
                font.family: Fonts.robotoCondensed
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
                font.bold: true
                font.pixelSize: 16
            }
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

import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0

Rectangle {
    id: idBannerVectorInfo
    height: 60
    Layout.fillWidth: true
    Layout.fillHeight: true

    border.width: 1
    border.color: "black"

    property alias idBtPositionPlus: btPositionPlus
    property alias idBtPositionMinus: btPositionMinus
    property alias idLePosition: lePosition
    property alias btVectorConnection: btVectorConnection

    RowLayout {
        id: rowBannerVectorInfo
        anchors.fill: parent
        anchors.leftMargin: 5

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

            onPressed: {
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

        Item {
            id: itSpacerHorizontal
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Rectangle {
            // seaparator
            width: 5
            Layout.fillHeight: true
            color: Design.colSeparator
            Layout.topMargin: 5
            Layout.bottomMargin: 5
        }

        // -------------------
        // Peripherals
        Image {
            id: icJoystick
            Layout.fillHeight: true
            Layout.topMargin: 5
            Layout.bottomMargin: 5
            source: joystickManager.isConnected ? "qrc:///img/icons/iconJoystickConnected.png" : "qrc:///img/icons/iconJoystickDisconnected.png"
            fillMode: Image.PreserveAspectFit
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
            }
        }

        Rectangle {
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
            ToolTip.text: qsTr("Etat : " + (2 === currentVector.FuncStateConnection ? qsTr("Connecté") : 1 === currentVector.FuncStateConnection ? qsTr("Partiellement connecté") : qsTr("Déconnecté")));
        }

        Button {
            id: btVectorEmergencyStop

            Layout.preferredWidth: idBannerVectorInfo.height
            Layout.preferredHeight: idBannerVectorInfo.height

            icon.source: "qrc:///img/icons/btEmergencyStop.png"
            icon.width: idBannerVectorInfo.height
            icon.height: idBannerVectorInfo.height
            icon.color: "transparent"
            padding: 2

            background: Rectangle {
                color: btVectorEmergencyStop.pressed ? "#FF0000" : btVectorEmergencyStop.hovered ? "#FFAE4C" : "transparent"
                // keep 1 pixel clean arround the button to not override the layout frame
                width: btVectorEmergencyStop.width-2
                x : 1
                height: btVectorEmergencyStop.height-2
                y : 1
            }

            onPressed: {
                currentVector.emergencyStop();
            }
        }
    }

    //    idLePosition.text : currentVector.funcPosition().paramLBscanPosMmX
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
        target : currentVector
        function onEvFuncStateConnectionChanged() {
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
        idLePosition.text = currentVector.funcPosition().getBScanPosMeterWith2Decimals(); //(Math.round(currentVector.funcPosition().paramLBscanPosMmX/10)/100);
    }

}

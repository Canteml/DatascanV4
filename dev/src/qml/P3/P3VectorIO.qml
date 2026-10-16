import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0
import Fonts 1.0

Rectangle {
    color: "#FFFFFF"
    Layout.preferredWidth: 160
    visible: examManager.controlType !== 2 // PIPE (Panda V3)
    border.width: 1
    border.color: Design.colFrameBorder


    ColumnLayout {
        id: idColBScanCtrl
        anchors.fill: parent
        anchors.margins: 10
        spacing: 6

        Text {
            text: qsTr("ACCESSOIRES")
            font.family: Fonts.robotoCondensed
            font.bold: true
            font.pixelSize: 12
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: qsTr("POMPE COUPLANT")
            font.family: Fonts.robotoCondensed
            font.bold: true
            font.pixelSize: 12
            Layout.alignment: Qt.AlignHCenter
        }


        /*
        RowLayout {
            //visible: currentVector.funcPump().isEnabled
            visible: true
            Layout.preferredHeight: 30
            Layout.fillWidth: true
            Button {
                id: btPumpMinus
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                background: Rectangle {
                    color: (btPumpMinus.pressed) ? Design.colButtonSelectedBackground : btPumpMinus.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                    border.width: 1
                    border.color: Design.colFrameBorder
                }
                text: qsTr("-")
                font.pointSize: 12
                onPressed: {
                    keyAction();
                }
                Action {
                    shortcut: "-"
                    onTriggered: btPumpMinus.keyAction()
                }
                Action {
                    shortcut: "_"
                    onTriggered: btPumpMinus.keyAction()
                }

                function keyAction() {
                    var v = currentVector.funcPump().paramLPumpLevel;
                    if( v>10 ) {
                        currentVector.funcPump().paramLPumpLevel = v - 10
                    }
                }
            }

            Button {
                id: btPumpEnable
                Layout.fillWidth: true
                Layout.preferredHeight: 30
                text: qsTr("POMPE ") + currentVector.funcPump().paramLPumpLevel + " %"

                checked: currentVector.funcPump().paramLPumpEnable
                checkable: true

                background: Rectangle {
                    color: (btPumpEnable.pressed) ? Design.colButtonSelectedBackground : (btPumpEnable.checked) ?  "#2020A0" : btPumpEnable.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                    border.width: 1
                    border.color: Design.colFrameBorder
                }
                font.pointSize: 9

                Action {
                    shortcut: "p"
                    onTriggered: btPumpEnable.keyAction()
                }
                Action {
                    shortcut: "P"
                    onTriggered: btPumpEnable.keyAction()
                }

                onReleased:
                {
                    currentVector.funcPump().paramLPumpEnable = 1-currentVector.funcPump().paramLPumpEnable
                }

                function keyAction() {
                    currentVector.funcPump().paramLPumpEnable = 1-currentVector.funcPump().paramLPumpEnable
                }
            }

            Button {
                id: btPumpPlus
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                background: Rectangle {
                    color: (btPumpPlus.pressed) ? Design.colButtonSelectedBackground : btPumpPlus.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                    border.width: 1
                    border.color: Design.colFrameBorder
                }
                text: qsTr("+")

                font.pointSize: 12
                onPressed: {
                    keyAction()
                }
                Action {
                    shortcut: "+"
                    onTriggered: btPumpPlus.keyAction()
                }
                Action {
                    shortcut: "="
                    onTriggered: btPumpPlus.keyAction()
                }

                function keyAction() {
                    var v = currentVector.funcPump().paramLPumpLevel;
                    if( v<100 ) {
                        currentVector.funcPump().paramLPumpLevel = v + 10
                    }
                }
            }
        }

        RowLayout {
            //visible: currentVector.funcLight().isEnabled
            Layout.preferredHeight: 30
            Layout.fillWidth: true
            visible: true
            Button {
                id: btLightMinus
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                background: Rectangle {
                    color: (btLightMinus.pressed) ? Design.colButtonSelectedBackground : btLightMinus.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                    border.width: 1
                    border.color: Design.colFrameBorder
                }
                text: qsTr("-")
                font.pointSize: 12
                onPressed:
                {
                    var v = currentVector.funcLight().paramLLightLevel;
                    if( v>10 ) {
                        currentVector.funcLight().paramLLightLevel = v - 10
                    }
                }
            }

            Button {
                id: btLightEnable
                Layout.fillWidth: true
                Layout.preferredHeight: 30
                text: qsTr("LED ") + currentVector.funcLight().paramLLightLevel + " %"
                background: Rectangle {
                    color: (btLightEnable.pressed) ? Design.colButtonSelectedBackground : (btLightEnable.checked) ?  "#909000" : btLightEnable.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                    border.width: 1
                    border.color: Design.colFrameBorder
                }

                checked: currentVector.funcLight().paramLLightEnable
                checkable: true
                font.pointSize: 9

                onReleased:
                {
                    currentVector.funcLight().paramLLightEnable = 1-currentVector.funcLight().paramLLightEnable
                }
            }

            Button {
                id: btLightPlus
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                background: Rectangle {
                    color: (btLightPlus.pressed) ? Design.colButtonSelectedBackground : btLightPlus.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                    border.width: 1
                    border.color: Design.colFrameBorder
                }
                text: qsTr("+")
                font.pointSize: 12
                onPressed:
                {
                    var v = currentVector.funcLight().paramLLightLevel;
                    if( v<100 ) {
                        currentVector.funcLight().paramLLightLevel = v + 10
                    }
                }
            }
        }*/

        RowLayout {
            // visible: currentVector.funcCleaning().isEnabled
            visible: true
            Layout.preferredHeight: 30
            Layout.fillWidth: true
            Button {
                id: btCPumpMinus
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                background: Rectangle {
                    color: "white"
                    border.width: 2
                    border.color: Design.colFrameBorder
                }
                contentItem: Text {
                    text: qsTr("-")
                    font.family: Fonts.robotoCondensed
                    font.bold: true
                    color: Design.colMainInk
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                font.pointSize: 12
                onPressed:
                {
                    var v = currentVector.funcCleaning().paramLPumpLevel;
                    if( v>10 ) {
                        currentVector.funcCleaning().paramLPumpLevel = v - 10
                    }
                }
            }

            Button {
                id: btCPumpEnable
                Layout.fillWidth: true
                Layout.preferredHeight: 30

                contentItem: Text {
                    text: currentVector.funcCleaning().paramLPumpLevel + " %"
                    font.family: Fonts.robotoCondensed
                    font.bold: true
                    color: Design.colMainInk
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    color: "white"
                    border.width: 2
                    border.color: Design.colFrameBorder
                }

                checked: currentVector.funcCleaning().paramLPumpEnable
                checkable: true
                font.pointSize: 9

                onReleased:
                {
                    currentVector.funcCleaning().paramLPumpEnable = 1-currentVector.funcCleaning().paramLPumpEnable
                }
            }

            Button {
                id: btCPumpPlus
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                background: Rectangle {
                    color: "white"
                    border.width: 2
                    border.color: Design.colFrameBorder
                }
                contentItem: Text {
                    text: qsTr("+")
                    font.family: Fonts.robotoCondensed
                    font.bold: true
                    color: Design.colMainInk
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                font.pointSize: 12
                onPressed:
                {
                    var v = currentVector.funcCleaning().paramLPumpLevel;
                    if( v<100 ) {
                        currentVector.funcCleaning().paramLPumpLevel = v + 10
                    }
                }
            }
        }

        Text {
            text: qsTr("BROSSE")
            font.family: Fonts.robotoCondensed
            font.bold: true
            font.pixelSize: 12
            Layout.alignment: Qt.AlignHCenter
        }

        RowLayout {
            // visible: currentVector.funcCleaning().isEnabled
            visible: true
            Layout.preferredHeight: 30
            Layout.fillWidth: true
            Button {
                id: btBrushMinus
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                background: Rectangle {
                    color: "white"
                    border.width: 2
                    border.color: Design.colFrameBorder
                }
                contentItem: Text {
                    text: qsTr("-")
                    font.family: Fonts.robotoCondensed
                    font.bold: true
                    color: Design.colMainInk
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                font.pointSize: 12
                onPressed:
                {
                    var v = currentVector.funcCleaning().paramLBrushLevel;
                    if( v>10 ) {
                        currentVector.funcCleaning().paramLBrushLevel = v - 10
                    }
                }
            }

            Button {
                id: btBrushEnable
                Layout.fillWidth: true
                Layout.preferredHeight: 30
                contentItem: Text {
                    text: currentVector.funcCleaning().paramLBrushLevel + " %"
                    font.family: Fonts.robotoCondensed
                    font.bold: true
                    color: Design.colMainInk
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    color: "white"
                    border.width: 2
                    border.color: Design.colFrameBorder
                }

                checked: currentVector.funcCleaning().paramLBrushEnable
                checkable: true
                font.pointSize: 9

                onReleased:
                {
                    currentVector.funcCleaning().paramLBrushEnable = 1-currentVector.funcCleaning().paramLBrushEnable
                }
            }

            Button {
                id: btBrushPlus
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                background: Rectangle {
                    color: "white"
                    border.width: 2
                    border.color: Design.colFrameBorder
                }

                contentItem: Text {
                    text: qsTr("+")
                    font.family: Fonts.robotoCondensed
                    font.bold: true
                    color: Design.colMainInk
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                font.pointSize: 12
                onPressed:
                {
                    var v = currentVector.funcCleaning().paramLBrushLevel;
                    if( v<100 ) {
                        currentVector.funcCleaning().paramLBrushLevel = v + 10
                    }
                }
            }
        }

        Rectangle {
            // seaparator
            Layout.fillWidth: true
            Layout.topMargin: 4
            Layout.bottomMargin: 4
            height: 3
            color: "#2F5E72"
        }

        Text {
            Layout.alignment: Qt.AlignCenter
            text: qsTr("GAIN")
            font.family: Fonts.robotoCondensed
            font.pixelSize: 12
            font.bold: true
            Layout.topMargin: 5
        }

        RowLayout {
            // visible: currentVector.funcCleaning().isEnabled
            visible: true
            Layout.preferredHeight: 30
            Layout.fillWidth: true

            P3RepeatButton {
                id: idBtGainMinus
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                enabled: scanManager.SelectedFuncAScan.paramDGain > 0

                background: Rectangle {
                    color: "white"
                    border.width: idBtGainMinus.enabled ? 2 : 0
                    border.color: Design.colFrameBorder
                }

                contentItem: Text {
                    text: qsTr("-")
                    font.family: Fonts.robotoCondensed
                    font.pointSize: 12
                    font.bold: true
                    color: Design.colButtonDarkBackground
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onStepped: {
                    if(scanManager.SelectedFuncAScan.paramDGain - 1 < 0){
                        scanManager.SelectedFuncAScan.paramDGain = 0
                    }
                    else
                        scanManager.SelectedFuncAScan.paramDGain = scanManager.SelectedFuncAScan.paramDGain - 1
                }
            }

            Text {
                text: scanManager.SelectedFuncAScan.paramDGain + " dB"
                font.family: Fonts.robotoCondensed
                font.pixelSize: 12
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }

            P3RepeatButton {
                id: idBtGainPlus
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                enabled: scanManager.SelectedFuncAScan.paramDGain < 80

                background: Rectangle {
                    color: "white"
                    border.width: idBtGainPlus.enabled ? 2 : 0.5
                    border.color: Design.colFrameBorder
                }

                contentItem: Text {
                    text: qsTr("+")
                    font.family: Fonts.robotoCondensed
                    font.pointSize: 12
                    font.bold: true
                    color: Design.colButtonDarkBackground
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onStepped: {
                    if(scanManager.SelectedFuncAScan.paramDGain + 1 > 80){
                        scanManager.SelectedFuncAScan.paramDGain = 80
                    }
                    else
                        scanManager.SelectedFuncAScan.paramDGain = scanManager.SelectedFuncAScan.paramDGain + 1
                }
            }
        }

        Text {
            Layout.alignment: Qt.AlignCenter
            text: qsTr("Résol. x")
            font.family: Fonts.robotoCondensed
            font.pixelSize: 12
            font.bold: true
        }

        RowLayout {
            Layout.preferredHeight: 30
            Layout.fillWidth: true

            P3RepeatButton {
                id: idBtResolutionMinus
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                Layout.rightMargin: 5

                background: Rectangle {
                    color: "white"
                    border.width: 2
                    border.color: Design.colFrameBorder
                }

                contentItem: Text {
                    text: qsTr("-")
                    font.family: Fonts.robotoCondensed
                    font.pointSize: 12
                    font.bold: true
                    color: Design.colButtonDarkBackground
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onStepped: {
                    scanManager.ascanZoomOut()
                }
            }

            Item { Layout.fillWidth: true }

            P3RepeatButton {
                id: idBtResolutionPlus
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30

                background: Rectangle {
                    color: "white"
                    border.width: 2
                    border.color: Design.colFrameBorder
                }

                contentItem: Text {
                    text: qsTr("+")
                    font.family: Fonts.robotoCondensed
                    font.pointSize: 12
                    font.bold: true
                    color: Design.colButtonDarkBackground
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onStepped: {
                    scanManager.ascanZoomIn()
                }
            }
        }

        Rectangle {
            // seaparator
            Layout.fillWidth: true
            Layout.topMargin: 4
            Layout.bottomMargin: 4
            height: 3
            color: "#2F5E72"
        }

        Button {
            Layout.fillWidth: true
            height: 75
            background: Rectangle {
                color: "white"
                border.width: 2
                border.color: Design.colFrameBorder
            }

            contentItem: Text {
                text: qsTr("OBSTACLE")
                font.family: Fonts.robotoCondensed
                font.pointSize: 12
                font.bold: true
                color: Design.colMainInk
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onPressed: {

            }
        }

        Button {
            Layout.fillWidth: true
            height: 75
            background: Rectangle {
                color: Design.colButtonDarkBackground
                border.width: 2
                border.color: Design.colFrameBorder
            }

            contentItem: Text {
                text: qsTr("SUPERVISEUR")
                font.family: Fonts.robotoCondensed
                font.pointSize: 12
                font.bold: true
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onPressed: {

            }
        }

        Item {
            // Spacer
            Layout.fillHeight: true
        }

        // -- MOVE
        ColumnLayout {
            focus: true
            id: keyLayout
            visible : currentVector.funcMove().isEnabled
            Layout.fillWidth: true
            spacing: 5

            RowLayout {
                focus: true

                Button {
                    visible: false
                    id: idBtMoveAuto
                    icon.source: "qrc:///img/icons/on_wall.png"
                    icon.width: 50
                    icon.height: 50
                    icon.color: "transparent"

                    antialiasing: true

                    onClicked: {
                        if( currentVector.funcMove().IsManualMode !== false ) {
                            currentVector.funcMove().setManualMode(false); // SEMI AUTO
                        } else {
                            currentVector.funcMove().setManualMode(true); // MANUAL
                        }
                    }

                    background: Rectangle {
                        color: (currentVector.funcMove().IsManualMode === false) ?
                                   Design.colGreen : "white" //Design.colButtonBackground
                        border.color: Design.colFrameBorder
                        radius: 10
                    }


                }

                Item {
                    // Spacer
                    Layout.fillWidth: true
                }

                /*
                Button {
                    icon.source: "qrc:///img/icons/btAutoMove.png"
                    icon.width: 50
                    icon.height: 50
                    icon.color: "transparent"

                    antialiasing: true
                    enabled: currentVector.funcMove().IsManualMode == true
                    onClicked: {
                      currentVector.funcMove().setManualMode(false);
                    }
                    //onClicked: currentVector.funcMove().IsmanualMode = 0
                    background: Rectangle {
                        color: parent.enabled ? "white" : "#00FF00"
                        border.color: Design.colFrameBorder
                        radius: 10
                    }
                }
                */
            }

            // -- LINE UP
            RowLayout {
                focus: true
                Item{
                    Layout.fillWidth : true;
                }

                Button {
                    id: arrow_up
                    focus: true
                    icon.source: "qrc:///img/icons/btArrowKeyUp.png"
                    icon.width: 30
                    icon.height: 30
                    icon.color: (currentVector.funcMove().moveDir & 0b0100 ) ? "white" : "black"
                    Layout.minimumWidth: 40
                    Layout.minimumHeight: 40

                    background: Rectangle {
                       id: arrow_upRect
                       color: (currentVector.funcMove().moveDir & 0b0100 ) ? Design.colBlueSelected : parent.hovered ? Design.colButtonHoveredBackground : "white" //Design.colButtonBackground
                       border.color: Design.colFrameBorder
                       border.width: 3
                    }

                    onPressed: {
                       currentVector.funcMove().moveKeyUpState(true);
                    }
                    onReleased: {
                       currentVector.funcMove().moveKeyUpState(false);
                    }

                }

                Item{
                    Layout.fillWidth : true;
                }
            }

            // -- LINE BOTTOM
           RowLayout {
               focus: true
               spacing: 5

                Item{
                    Layout.fillWidth : true;
                }

                Button {
                   id: arrow_left
                   icon.source: "qrc:///img/icons/btArrowKeyLeft.png"
                   icon.width: 30
                   icon.height: 30
                   icon.color: (currentVector.funcMove().moveDir & 0b0001 ) ? "white" : "black"
                   Layout.minimumWidth: 40
                   Layout.minimumHeight: 40

                   background: Rectangle {
                       id: arrow_leftRect
                       color: (currentVector.funcMove().moveDir & 0b0001 ) ? Design.colBlueSelected : parent.hovered ? Design.colButtonHoveredBackground : "white" //Design.colButtonBackground
                       border.color: Design.colFrameBorder
                       border.width: 3
                   }

                   onPressed: {
                       currentVector.funcMove().moveKeyLeftState(true);
                   }
                   onReleased: {
                       currentVector.funcMove().moveKeyLeftState(false);
                   }
                }

                Button {
                   id: arrow_down
                   icon.source: "qrc:///img/icons/btArrowKeyDown.png"
                   icon.width: 30
                   icon.height: 30
                   icon.color: (currentVector.funcMove().moveDir & 0b1000 ) ? "white" : "black"
                   Layout.minimumWidth: 40
                   Layout.minimumHeight: 40

                   background: Rectangle {
                       id: arrow_downRect
                       color: (currentVector.funcMove().moveDir & 0b1000 ) ? Design.colBlueSelected : parent.hovered ? Design.colButtonHoveredBackground : "white" //Design.colButtonBackground
                       border.color: Design.colFrameBorder
                       border.width: 3
                   }

                   onPressed: {
                       currentVector.funcMove().moveKeyDownState(true);
                   }
                   onReleased: {
                       currentVector.funcMove().moveKeyDownState(false);
                   }
                }



                Button {
                   id: arrow_right
                   icon.source: "qrc:///img/icons/btArrowKeyRight.png"
                   icon.width: 30
                   icon.height: 30
                   icon.color: (currentVector.funcMove().moveDir & 0b0010 ) ? "white" : "black"
                   Layout.minimumWidth: 40
                   Layout.minimumHeight: 40

                   background: Rectangle {
                       id: arrow_rightRect
                       color: (currentVector.funcMove().moveDir & 0b0010 ) ? Design.colBlueSelected : parent.hovered ? Design.colButtonHoveredBackground : "white" //Design.colButtonBackground
                       border.color: Design.colFrameBorder
                       border.width: 3
                   }

                   onPressed: {
                       currentVector.funcMove().moveKeyRightState(true);
                   }
                   onReleased: {
                       currentVector.funcMove().moveKeyRightState(false);
                   }
               }

                Item{
                    Layout.fillWidth : true;
                }
            }

        }

    }


}





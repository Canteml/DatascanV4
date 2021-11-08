import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0

Rectangle {
    color: "#FFFFFF"
    Layout.preferredWidth: 200

    ColumnLayout {
        id: idColBScanCtrl
        anchors.fill: parent
        anchors.margins: 5

        RowLayout {
            visible: currentVector.funcPump().isEnabled
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
                onPressed:
                {
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

                onReleased:
                {
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
                onPressed:
                {
                    var v = currentVector.funcPump().paramLPumpLevel;
                    if( v<100 ) {
                        currentVector.funcPump().paramLPumpLevel = v + 10
                    }
                }
            }
        }

        RowLayout {
            visible: currentVector.funcLight().isEnabled
            Layout.preferredHeight: 30
            Layout.fillWidth: true
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
        }

        Item {
            // Spacer
            Layout.fillHeight: true
        }

        // -- MOVE
        ColumnLayout {
            id: keyLayout
            visible : currentVector.funcMove().isEnabled
            spacing: 5

            // -- LINE UP
            RowLayout {
                Item{
                    Layout.fillWidth : true;
                }

                Button {
                    id: arrow_up
                    icon.source: "qrc:///img/icons/btArrowKeyUp.png"
                    icon.width: 30
                    icon.height: 30
                    Layout.minimumWidth: 45
                    Layout.minimumHeight: 45

                    background: Rectangle {
                       id: arrow_upRect
                       color: (currentVector.funcMove().moveDir & 0b0100 ) ? Design.colBlueSelected : parent.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                       border.color: Design.colFrameBorder
                       border.width: 1
                       radius: 4
                    }

                    onPressed: {
                       currentVector.funcMove().moveKeyUp();
                    }
                    onReleased: {
                       currentVector.funcMove().moveStopY();
                    }
                }

                Item{
                    Layout.fillWidth : true;
                }
            }

            // -- LINE MIDDLE
           RowLayout {
                spacing: 5

                Item{
                    Layout.fillWidth : true;
                }

                Button {
                   id: arrow_left
                   icon.source: "qrc:///img/icons/btArrowKeyLeft.png"
                   icon.width: 30
                   icon.height: 30
                   Layout.minimumWidth: 45
                   Layout.minimumHeight: 45

                   background: Rectangle {
                       id: arrow_leftRect
                       color: (currentVector.funcMove().moveDir & 0b0001 ) ? Design.colBlueSelected : parent.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                       border.color: Design.colFrameBorder
                       border.width: 1
                       radius: 4
                   }

                   onPressed: {
                       currentVector.funcMove().moveKeyLeft();
                   }
                   onReleased: {
                       currentVector.funcMove().moveStopX();
                   }
               }

                Button {
                   id: arrow_down
                   icon.source: "qrc:///img/icons/btArrowKeyDown.png"
                   icon.width: 30
                   icon.height: 30
                   Layout.minimumWidth: 45
                   Layout.minimumHeight: 45

                   background: Rectangle {
                       id: arrow_downRect
                       color: (currentVector.funcMove().moveDir & 0b1000 ) ? Design.colBlueSelected : parent.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                       border.color: Design.colFrameBorder
                       border.width: 1
                       radius: 4
                   }

                   onPressed: {
                       currentVector.funcMove().moveKeyDown();
                   }
                   onReleased: {
                       currentVector.funcMove().moveStopY();
                   }
                }



                Button {
                   id: arrow_right
                   icon.source: "qrc:///img/icons/btArrowKeyRight.png"
                   icon.width: 30
                   icon.height: 30
                   Layout.minimumWidth: 45
                   Layout.minimumHeight: 45

                   background: Rectangle {
                       id: arrow_rightRect
                       color: (currentVector.funcMove().moveDir & 0b0010 ) ? Design.colBlueSelected : parent.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                       border.color: Design.colFrameBorder
                       border.width: 1
                       radius: 4
                   }

                   onPressed: {
                       currentVector.funcMove().moveKeyRight();
                   }
                   onReleased: {
                       currentVector.funcMove().moveStopX();
                   }
               }

                Item{
                    Layout.fillWidth : true;
                }
            }

        }

    }


}





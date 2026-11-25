import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0

import AScanGraph 1.0

Rectangle {
    id: idRectP3TopAScan
    Layout.preferredWidth: 70

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            border.color: "#000000"

            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                spacing: 0

                anchors.fill : parent

                // -------------------
                // -- Top Banner

                RowLayout {
                    id: idAscanBanner
                    Layout.fillWidth: true
                    Layout.maximumHeight: 70
                    Layout.leftMargin: 5
                    Layout.rightMargin: 5

                    Image {
                        id: idThickness

                        Layout.topMargin: 5
                        Layout.bottomMargin: 5
                        Layout.maximumHeight: 60
                        Layout.maximumWidth:  60
                        source: "qrc:///img/icons/iconThicknessMm.png"
                        fillMode: Image.PreserveAspectFit
                    }

                    Text {
                        id: leAScanThickness
                        Layout.minimumWidth: 120
                        text: "12.34"
                        font.family: "Helvetica"
                        font.pixelSize: 48
                        horizontalAlignment: Text.AlignRight
                        color: "black"
                        smooth: true
                    }

                    Text {
                        id: leAScanThickness1E
                        Layout.fillHeight: true
                        Layout.bottomMargin: 12
                        text: " / 12.34"
                        font.family: "Helvetica"
                        font.pixelSize: 24
                        verticalAlignment: Text.AlignBottom
                        color: "red"
                        smooth: true
                    }

                    Connections {
                        target: scanManager
                        function onEvUpdateAScan() {
                            leAScanThickness.text = (Math.round(scanManager.getAScanThicknessMmMultiEcho()*10)/10).toFixed(1);
                            leAScanThickness1E.text = " / " + (Math.round(scanManager.getAScanThicknessMm1Echo()*10)/10).toFixed(1);
                        }
                    }

                    Item {
                        // Spacer
                        Layout.fillWidth: true
                    }

                    GridLayout {
                        columns: 2
                        visible: currentVector.NbFuncAScanEnabled > 1 && !cameraManager.selectMainCameraAuto

                        Button {
                            id: idBtAScan1
                            Layout.preferredHeight: 25
                            Layout.preferredWidth: 35
                            background: Rectangle {
                                color: (idBtAScan1.pressed
                                        || ( scanManager.SelectedAScanIndex
                                            === 0)) ? Design.colButtonBackground : idBtAScan1.hovered ? Design.colButtonHoveredBackground : "white"
                                border.width: 1
                                border.color: Design.colFrameBorder
                            }

                            contentItem: Text {
                                text: "1"
                                font.family: Fonts.robotoCondensed
                                font.pointSize: 12
                                font.bold: true
                                color:  (idBtAScan1.pressed
                                         || ( scanManager.SelectedAScanIndex
                                             === 0)) ? "white" : "black"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            onPressed: {
                                scanManager.SelectedAScanIndex = 0
                            }
                        }

                        Button {
                            id: idBtAScan2
                            Layout.preferredHeight: 25
                            Layout.preferredWidth: 35
                            background: Rectangle {
                                color: (idBtAScan2.pressed
                                        || ( scanManager.SelectedAScanIndex
                                            === 1)) ? Design.colButtonBackground : idBtAScan2.hovered ? Design.colButtonHoveredBackground : "white"
                                border.width: 1
                                border.color: Design.colFrameBorder
                            }

                            contentItem: Text {
                                text: "2"
                                font.family: Fonts.robotoCondensed
                                font.pointSize: 12
                                font.bold: true
                                color:  (idBtAScan2.pressed
                                         || ( scanManager.SelectedAScanIndex
                                             === 1)) ? "white" : "black"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }

                            onPressed: {
                                scanManager.SelectedAScanIndex = 1
                            }
                        }

                        Button {
                            id: idBtAScan3
                            visible: currentVector.NbFuncAScanEnabled > 2
                            Layout.preferredHeight: 25
                            Layout.preferredWidth: 35
                            background: Rectangle {
                                color: (idBtAScan3.pressed
                                        || ( scanManager.SelectedAScanIndex
                                            === 2)) ? Design.colButtonBackground : idBtAScan3.hovered ? Design.colButtonHoveredBackground : "white"
                                border.width: 1
                                border.color: Design.colFrameBorder
                            }

                            contentItem: Text {
                                text: "3"
                                font.family: Fonts.robotoCondensed
                                font.pointSize: 12
                                font.bold: true
                                color:  (idBtAScan3.pressed
                                         || ( scanManager.SelectedAScanIndex
                                             === 2)) ? "white" : "black"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }

                            onPressed: {
                                scanManager.SelectedAScanIndex = 2
                            }
                        }

                        Button {
                            id: idBtAScan4
                            visible: currentVector.NbFuncAScanEnabled > 3
                            Layout.preferredHeight: 25
                            Layout.preferredWidth: 35
                            background: Rectangle {
                                color: (idBtAScan4.pressed
                                        || ( scanManager.SelectedAScanIndex
                                            === 3)) ? Design.colButtonBackground : idBtAScan4.hovered ? Design.colButtonHoveredBackground : "white"
                                border.width: 1
                                border.color: Design.colFrameBorder
                            }

                            contentItem: Text {
                                text: "4"
                                font.family: Fonts.robotoCondensed
                                font.pointSize: 12
                                font.bold: true
                                color: (idBtAScan4.pressed
                                        || ( scanManager.SelectedAScanIndex
                                            === 3)) ? "white" : "black"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            onPressed: {
                                scanManager.SelectedAScanIndex = 3
                            }
                        }
                    }
                }

                // -------------------
                // -- ASCAN

                AScanGraph {
                    objectName: "AScanGraphMain"
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.margins: 2
                }
            }
        }
    }

    Image {
        source: (("qrc:///img/icons/schPipeAngle") + (examPipeManager.currentSensorNumber) + (".png"));
        visible: examManager.controlType === 2 /// Panda
        anchors.right: idRectP3TopAScan.right
        anchors.top: idRectP3TopAScan.top
        anchors.topMargin: 5
        anchors.rightMargin: 5
        height: 140
        fillMode: Image.PreserveAspectFit
        mipmap : true
        antialiasing : true
        smooth: true
    }
}
/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

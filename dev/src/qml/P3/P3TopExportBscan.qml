import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0

ColumnLayout {
    id: idP3TopP1
    Layout.fillWidth: true
    Layout.fillHeight: true

    property int exportStep: idSLExport.currentIndex

    StackLayout {
        id: idSLExport


        // -- PAGE 0

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: exportStep === 0

            // SPACER vertical
            Item {
                Layout.fillHeight: true
            }

            RowLayout {
                Layout.fillWidth: true


                // SPACER horizontal
                Item {
                    Layout.fillWidth: true
                }

                // Button Cancel
                Button {
                    id: btCancel

                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 200

                    icon.source: "qrc:///img/icons/iconCancel.png"
                    icon.width: 150
                    icon.height: 150
                    icon.color: "transparent"

                    antialiasing: true

                    background: Rectangle {
                        color: btCancel.pressed ? Design.colButtonSelectedBackground : btCancel.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        border.width: 2
                        border.color: Design.colButtonBorder
                        radius: 40
                    }

                    onPressed: {
                        examManager.cancelExamWrite();
                    }
                }


                // SPACER horizontal
                Item {
                    Layout.fillWidth: true
                }

                // Button ok
                Button {
                    id: btOk

                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 200

                    icon.source: "qrc:///img/icons/iconOk.png"
                    icon.width: 150
                    icon.height: 150
                    icon.color: "transparent"

                    antialiasing: true

                    background: Rectangle {
                        color: btOk.pressed ? Design.colButtonSelectedBackground : btOk.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        border.width: 2
                        border.color: Design.colButtonBorder
                        radius: 40
                    }

                    onPressed: {
                        examManager.startExamWrite(idTxtFileName.text)
                        txtExamFileName.text = examManager.getExamFileName()
                        idSLExport.currentIndex = 1;
                    }
                }

                // SPACER horizontal
                Item {
                    Layout.fillWidth: true
                }

            }

            // SPACER vertical
            Item {
                Layout.fillHeight: true
            }

            RowLayout {
                Layout.fillWidth: true

                // SPACER horizontal
                Item {
                    Layout.fillWidth: true
                }

                Text {
                    text: qsTr("Nom du CSV : ")
                    font.bold: true
                    font.pointSize: 14
                }

                TextField {
                    id: idTxtFileName
                    selectByMouse: true
                    Layout.preferredHeight: 40
                    Layout.preferredWidth: idP3Top.width/3
                    font.pointSize: 14


                    Component.onCompleted: {
                        idTxtFileName.text = examManager.bscanPerimeter.toFixed(2)
                    }

                    Connections {
                        target : examManager
                        function onExamStepChanged() {
                            idTxtFileName.text = examManager.getExamGeneratedFileName();
                        }
                    }
                }

                // SPACER horizontal
                Item {
                    Layout.fillWidth: true
                }
            }

            // SPACER vertical
            Item {
                Layout.fillHeight: true
            }

        }

        // -- STEP 1 : WRITE PROGRESS
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: exportStep === 1

            // SPACER vertical
            Item {
                Layout.fillHeight: true
            }

            RowLayout {
                Layout.fillWidth: true

                // SPACER horizontal
                Item {
                    Layout.fillWidth: true
                }

                Text {
                    text: qsTr("Sauvegarde du BSCAN : ")
                    font.bold: true
                    font.pointSize: 14
                }

                Text {
                    id: txtExamFileName
                    font.pointSize: 14
                }

                // SPACER horizontal
                Item {
                    Layout.fillWidth: true
                }
            }


            RowLayout {
                Layout.fillWidth: true

                // SPACER horizontal
                Item {
                    Layout.fillWidth: true
                }

                ProgressBar {
                    id: idPbExport
                    Layout.preferredWidth: 400
                    Layout.minimumHeight: 50
                    from: 0
                    to: 100
                    value: 20 //examManager.examWriteProgress

                    background: Rectangle {
                        implicitWidth: 400
                        implicitHeight: 50
                        color: Design.colButtonHoveredBackground
                        radius: 3
                    }

                    contentItem: Item {
                            implicitWidth: 400
                            implicitHeight: 48

                            Rectangle {
                                width: idPbExport.visualPosition * parent.width
                                height: parent.height
                                radius: 2
                                color: Design.colOrangeSelected
                            }
                        }
                }

                // SPACER horizontal
                Item {
                    Layout.fillWidth: true
                }
            }

            // SPACER vertical
            Item {
                Layout.fillHeight: true
            }

        }
    }
}

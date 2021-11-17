import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0

Rectangle {
    color: "#EFEFEF"
    border.color: "#000000"

    ColumnLayout {
        id: idColPipeCtrl
        anchors.fill: parent
        anchors.topMargin: 5

        RowLayout {
//            Layout.fillWidth: true;
//            Layout.preferredHeight: 28
            spacing: 5
            Layout.leftMargin: 5

            Button {
                id: idBtPipeNew
                Layout.preferredHeight: 28
                Layout.preferredWidth: 28
                antialiasing: true

                background: Rectangle {
                    color: idBtPipeNew.pressed ? Design.colButtonSelectedBackground : idBtPipeNew.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                    border.width: 1
                    border.color: Design.colButtonBorder
                    radius: 3
                }

                icon.source: "qrc:///img/icons/fileNew.png"
                icon.color: "transparent"
                icon.height: 22
                icon.width: 22
                padding: 1

                onPressed: {
                    examPipeManager.resetExam()
                }
            }

            Button {
                id: idBtPipeSave
                Layout.preferredHeight: 28
                Layout.preferredWidth: 28
                antialiasing: true

                background: Rectangle {
                    color: idBtPipeSave.pressed ? Design.colButtonSelectedBackground : idBtPipeSave.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                    border.width: 1
                    border.color: Design.colButtonBorder
                    radius: 3
                }

                icon.source: "qrc:///img/icons/fileSave.png"
                icon.color: "transparent"
                icon.height: 22
                icon.width: 22
                padding: 1

                onPressed: {
                    examPipeManager.exportExam("");
                }
            }
        }

        Rectangle {
            // Separator
            Layout.leftMargin: 5
            Layout.rightMargin: 5
            Layout.fillWidth: true
            Layout.preferredHeight: 2
            color : Design.colSeparator
        }

        Button {
            id: idBtPipeAcq
            Layout.preferredHeight: 60
            Layout.preferredWidth: 60
            Layout.leftMargin: 5
            antialiasing: true

            background: Rectangle {
                color: idBtPipeAcq.pressed ? Design.colButtonSelectedBackground : idBtPipeAcq.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                border.width: 1
                border.color: Design.colButtonBorder
                radius: 5
            }

            icon.source: "qrc:///img/icons/iconThicknessMm.png"
            icon.color: "transparent"
            icon.height: 50
            icon.width: 50
            padding: 1

            onPressed: {
                examPipeManager.takeMeasure();
            }
        }

        Button {
            id: idBtPipePicture
            Layout.preferredHeight: 60
            Layout.preferredWidth: 60
            Layout.leftMargin: 5
            antialiasing: true

            background: Rectangle {
                color: idBtPipePicture.pressed ? Design.colButtonSelectedBackground : idBtPipePicture.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                border.width: 1
                border.color: Design.colButtonBorder
                radius: 5
            }

            icon.source: "qrc:///img/icons/iconCamera.png"
            icon.color: "transparent"
            icon.height: 50
            icon.width: 50
            padding: 1

            onPressed: {
                examPipeManager.takePicture();
            }
        }

        Button {
            id: idBtPipeNext
            Layout.preferredHeight: 30
            Layout.preferredWidth: 60
            Layout.leftMargin: 5
            antialiasing: true

            background: Rectangle {
                color: (idBtPipeNext.pressed || currentVector.funcLight().LLightEnable) ? Design.colButtonSelectedBackground : idBtPipeNext.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                border.width: 1
                border.color: Design.colButtonBorder
                radius: 5
            }

            icon.source: "qrc:///img/icons/iconNextAcq.png"
            icon.color: "transparent"
            icon.height: 25
            icon.width: 25
            padding: 1

            onPressed: {
                examPipeManager.nextAcquisition();
            }
        }

        Rectangle {
            // Separator
            Layout.leftMargin: 5
            Layout.rightMargin: 5
            Layout.fillWidth: true
            Layout.preferredHeight: 2
            color : Design.colSeparator
        }

        Button {
            id: idBtPipeLight
            Layout.preferredHeight: 60
            Layout.preferredWidth: 60
            Layout.leftMargin: 5
            antialiasing: true

            checked: currentVector.funcLight().paramLLightEnable
            checkable: true

            background: Rectangle {
                color: (idBtPipeLight.pressed) ? Design.colButtonSelectedBackground : (idBtPipeLight.checked) ?  "#909000" : idBtPipeLight.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                border.width: 1
                border.color: Design.colButtonBorder
                radius: 5
            }

            icon.source: "qrc:///img/icons/iconLight.png"
            icon.color: "transparent"
            icon.height: 50
            icon.width: 50
            padding: 1

            onReleased: {
                if( !currentVector.funcLight().paramLLightEnable ) {
                    currentVector.funcLight().paramLLightLevel = 100;
                    currentVector.funcLight().paramLLightEnable = true;
                } else {
                    currentVector.funcLight().paramLLightEnable = false;
                }
            }
        }

        Item {
            // Spacer
            Layout.fillHeight: true
        }
    }
}

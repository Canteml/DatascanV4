import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import QtQuick.Dialogs
import Design 1.0

Rectangle {
    id: idP3Top

    border.width: 1
    border.color: "black"

    property alias idSlP4TopCamSettings: idSettingsSelection

    QtObject {
        id: idSettingsSelection
        property int currentIndex: 0
        property bool visible: true
        property bool camVisible: false
    }


    StackLayout {
        id: idSlTop
        anchors.fill: parent
        currentIndex: 0 //examManager.examStep === 3 /*EXAM_EXPORTING*/? 1 : 0

        Connections {
            target : examManager
            function onExamStepChanged() {
                if( examManager.examStep === 3 ) {
                    idP3TopExportBscan.init();
                    idSlTop.currentIndex = 1;
                } else {
                    idSlTop.currentIndex = 0;
                }
            }
        }

        // -- PAGE ASCAN
        RowLayout {
            id: idP3TopP0
            Layout.fillWidth: true
            Layout.fillHeight: true

            spacing: 0

            // -------------------
            // -- Left :

            P3TopAscan {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: idSettingsSelection.camVisible ? (idP3TopP0.width - 70) / 2 : (idP3TopP0.width - 70)
            }

            // -------------------
            // -- Right : solo camera (i menu A-SCAN / B-SCAN / MEP / ETAT sono in P3Bottom)

            P3TopCam {
                id: idP3TopCam
                visible: idSettingsSelection.camVisible

                Layout.fillWidth: true
                Layout.preferredWidth: (idP3TopP0.width - 70) / 2
                Layout.fillHeight: true
            }
        }

        // -- PAGE EXPORT EXAM
        P3TopExportBscan {
            id: idP3TopExportBscan

        }
    }

    Connections {
        target: examManager
        function onControlTypeChanged() {
            idSettingsSelection.currentIndex = 0
        }
        // the CSV written during the acquisition can't be created / written
        function onEvExportErrorChanged(p_fileName) {
            dialogExportError.text = qsTr("Impossible d'écrire le fichier CSV de l'acquisition :\n") + p_fileName
            dialogExportError.visible = true
        }
    }

    MessageDialog {
        id: dialogExportError
        title: qsTr("Erreur fichier CSV")
        buttons: MessageDialog.Ok
    }

    Component.onCompleted: {
        idSettingsSelection.currentIndex = 0;
        idSettingsSelection.visible = true;
    }
}
/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

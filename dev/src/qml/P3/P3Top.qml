import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0

Rectangle {
    id: idP3Top

    border.width: 1
    border.color: "black"


    StackLayout {
        id: idSlTop
        anchors.fill: parent
        currentIndex: 0 //examManager.examStep === 3 /*EXAM_EXPORTING*/? 1 : 0

        Connections {
            target : examManager
            function onExamStepChanged() {
                if( examManager.examStep === 3 ) {
                    idP3TopExportBscan.exportStep = 0;  // init step
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
            // -- Left controls

            P3TopAscanCtrl {
                Layout.preferredWidth: 70
                Layout.fillHeight: true
            }

            // -------------------
            // -- Center :

            P3TopAscan {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            // -------------------
            // -- Right :

            P3TopCamSettings {
                // Layout.fillWidth: true => controled by P3TopCamSettings
                Layout.fillHeight: true
            }
        }

        // -- PAGE EXPORT EXAM
        P3TopExportBscan {
            id: idP3TopExportBscan

        }
    }
}
/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

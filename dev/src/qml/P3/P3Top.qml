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


    Item {
        id: idSlTop
        anchors.fill: parent
        property int currentIndex: 0 //examManager.examStep === 3 /*EXAM_EXPORTING*/? 1 : 0

        Connections {
            target : examManager
            function onExamStepChanged() {
                if( examManager.examStep === 3 ) {
                    idP3TopExportBscan.exportStep = 0;
                    idSlTop.currentIndex = 1;
                } else {
                    idSlTop.currentIndex = 0;
                }
            }
        }

        // -- PAGE ASCAN
        RowLayout {
            id: idP3TopP0
            anchors.fill: parent

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

        // -- MENU B-SCAN (over the ASCAN page, moved here from P3Bottom), shown while B-SCAN of P3SettingsBar is selected.
        // "FERMER" sets currentIndex = 0 => closed
        Rectangle {
            anchors.fill: parent
            visible: idSettingsSelection.visible && idSettingsSelection.currentIndex === 2
            color: "white"

            // block mouse / wheel events to the ASCAN page below
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onWheel: function(wheel) { wheel.accepted = true }
            }

            P3BottomSettingsBScan {
                anchors.fill: parent
                slCamSettings: idSettingsSelection
            }
        }

        // -- PAGE EXPORT EXAM (over the ASCAN page)
        Rectangle {
            anchors.fill: parent
            visible: idSlTop.currentIndex === 1
            color: "white"

            // block mouse / wheel events to the ASCAN page below
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onWheel: function(wheel) { wheel.accepted = true }
            }

            P3TopExportBscan {
                id: idP3TopExportBscan
                anchors.fill: parent
            }
        }
    }

    Connections {
        target: examManager
        function onControlTypeChanged() {
            idSettingsSelection.currentIndex = 0
        }
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

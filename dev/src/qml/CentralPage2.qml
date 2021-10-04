import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import "rp_components"

Rectangle {

    Layout.fillWidth: true
    Layout.fillHeight: true

    ColumnLayout {
        id: idP2MainLayout

        anchors.fill: parent
        anchors.margins: 5

        StackLayout {

            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: examManager.controlType

            //-- PAGE BSCAN
            ColumnLayout {
                id: idP2_BSCAN
                Layout.fillWidth: true
                Layout.fillHeight: true

                RowLayout {
                    id: idP1BscanGeneratrix
                    Layout.fillWidth: true

                    FormSection {
                        text: qsTr("Generatrice BSCAN")
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.leftMargin: 20

                    Text {
                        text: qsTr("Choix du numéro de la génératice à scanner :")
                        font.pixelSize: 14
                    }

                    SpinBox {
                        // the value starts from 1 but the property starts at 0
                        value : examManager.selectedBScanGeneratrix + 1
                        from : 1
                        to : examManager.bscanNbGeneratrix
                        enabled: examManager.examStep === 0 ///< can be changed only if no scan running

                        onValueChanged: {
                            if( (value - 1) !== examManager.selectedBScanGeneratrix ) {
                                examManager.selectedBScanGeneratrix = (value - 1);
                            }
                        }
                    }
                }
            }
        }


        // -- SPACER
        Item {
            // Spacer
            Layout.fillWidth: true
            Layout.fillHeight: true
        }


        // -- Bottom Banner

        BannerBottomNavigate {
            id: idBannerBottom2
            idBtPrev.text : qsTr("PREPARER")
            idBtNext.text : qsTr("SCANNER")

            Connections {
                target: idBannerBottom2.idBtPrev
                function onClicked() {
                    globalManager.guiPage = 1;
                }
            }

            Connections {
                target: idBannerBottom2.idBtNext
                function onClicked() {
                    globalManager.guiPage = 3;
                }
            }
        }
    }
}

import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

import Qt.labs.qmlmodels 1.0

Rectangle {

    Layout.fillWidth: true
    Layout.fillHeight : true

    RowLayout {
        spacing: 0
        anchors.fill: parent

        // Stack function to Exam control type
        StackLayout {
            id: idSlBottom
            currentIndex: examManager.controlType
            Layout.fillWidth: true

            // --------------------------------------------------------------------
            // -- PAGE BSCAN
            RowLayout {
                id: idBScan
                spacing: 0
                Layout.fillWidth: true

                P3BottomBScanCtrl {
                    Layout.preferredWidth: 70
                    Layout.fillHeight: true
                }

                Rectangle {
                    border.width: 1
                    border.color: "#000000"
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    P3BottomBScanView {
                        anchors.fill : parent
                        anchors.margins: 1
                    }
                }
            }

            // --------------------------------------------------------------------
            // -- PAGE MEP

            RowLayout {
                id: idMEP
                spacing: 0
                Layout.fillWidth: true

                Rectangle {
                    id: idMep
                    color: "#C9C9C9"
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    P3BottomMepView {
                        anchors.fill: parent
                        anchors.margins: 1
                    }

                }

            }

            // --------------------------------------------------------------------
            // -- PAGE PIPE
            RowLayout {
                id: idPipe // panda vol 3
                spacing: 0
                Layout.fillWidth: true

                P3BottomPipeCtrl {
                    Layout.preferredWidth: 70
                    Layout.fillHeight: true
                }

                Rectangle {
                    border.width: 1
                    border.color: "#000000"
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    P3BottomPipeView {
                        anchors.fill : parent
                        anchors.margins: 1
                    }
                }
            }
        }

        P3BottomVectorIO {
            Layout.fillHeight: true
        }
    }
}

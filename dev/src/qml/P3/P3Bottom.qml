import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

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

            Rectangle {
                id: idMep
                color: "#900010"
            }

            Rectangle {
                id: idPipe // panda vol 3
                color: "#12AA85"
            }
        }

        P3BottomVectorIO {
            Layout.fillHeight: true
        }
    }
}

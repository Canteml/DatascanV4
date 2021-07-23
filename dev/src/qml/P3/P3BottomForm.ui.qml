import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

Rectangle {

    color: "#501050"

    RowLayout {
        spacing: 0

        anchors.fill: parent

        P3BottomBScanCtrl {
            Layout.fillHeight: true
        }
        P3BottomBScanView {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        P3BottomVectorIO {
            Layout.fillHeight: true
        }
    }
}

import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

Rectangle {
    color: "#FFFFFF"
    Layout.preferredWidth: 200

    ColumnLayout {
        id: idColBScanCtrl
        anchors.fill: parent
        anchors.margins: 5

        RowLayout {
            visible: currentVector.funcPump().isEnabled
            Layout.preferredHeight: 30
            Layout.fillWidth: true
            Button {
                id: btPumpMinus
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                text: qsTr("-")
                onPressed:
                {
                    var v = currentVector.funcPump().paramLPumpLevel;
                    if( v>10 ) {
                        currentVector.funcPump().paramLPumpLevel = v - 10
                    }
                }
            }

            Button {
                id: btPumpEnable
                Layout.fillWidth: true
                Layout.preferredHeight: 30
                text: qsTr("POMPE ") + currentVector.funcPump().paramLPumpLevel + " %"

                checked: currentVector.funcPump().paramLPumpEnable
                checkable: true

                onReleased:
                {
                    currentVector.funcPump().paramLPumpEnable = 1-currentVector.funcPump().paramLPumpEnable
                }
            }

            Button {
                id: btPumpPlud
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                text: qsTr("+")
                onPressed:
                {
                    var v = currentVector.funcPump().paramLPumpLevel;
                    if( v<100 ) {
                        currentVector.funcPump().paramLPumpLevel = v + 10
                    }
                }
            }
        }

        Item {
            // Spacer
            Layout.fillHeight: true
        }
    }


}

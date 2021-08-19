import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0
import "../rp_components"

Rectangle {
    color: "#EFEFEF"
    border.color: "#000000"

    ColumnLayout {
        id: idColAScanCtrl

        Text {
            Layout.alignment: Qt.AlignCenter
            text: "Gain"
            font.pixelSize: 20
            Layout.topMargin: 5
        }

        SpinBox {
            id: sbGain

            from: 0
            to: 80
            stepSize: 1
            editable: true

            value: scanManager.SelectedFuncAScan.paramDGain

            onValueChanged: {
                scanManager.SelectedFuncAScan.paramDGain = value
            }
            onValueModified: {
                focus = false;
            }

            Layout.alignment: Qt.AlignCenter
            Layout.leftMargin: 5

            implicitHeight: 64
            implicitWidth: 60

            contentItem: TextInput {
                text: sbGain.textFromValue(sbGain.value, sbGain.locale)

                anchors.fill: parent
                anchors.bottomMargin: 30


                font.pointSize: 12
                color: "#000000"

                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter

                readOnly: !sbGain.editable
                validator: sbGain.validator
                inputMethodHints: Qt.ImhFormattedNumbersOnly

            }

            up.indicator: Rectangle {
                height: parent.height/2
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                implicitHeight: 30
                implicitWidth: 30 // Adjust width here
                color: sbGain.up.pressed ? Design.colButtonSelectedBackground : Design.colButtonBackground
                border.color: enabled ? Design.colFrameBorder : Design.colButtonHoveredBackground
                Text {
                    text: '+'
                    font.pointSize: 12
                    anchors.centerIn: parent
                    color : parent.enabled ? Design.colButtonSelectedText : Design.colButtonSelectedBackground
                }
            }
            down.indicator: Rectangle {
                height: parent.height/2
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                implicitHeight: 30
                implicitWidth: 30 // Adjust width here
                color: sbGain.down.pressed ? Design.colButtonSelectedBackground : Design.colButtonBackground
                border.color: enabled ? Design.colFrameBorder : Design.colButtonHoveredBackground
                Text {
                    text: '-'
                    font.pointSize: 12
                    anchors.centerIn: parent
                    color : parent.enabled ? Design.colButtonSelectedText : Design.colButtonSelectedBackground
                }
            }

        }

        Text {
            Layout.alignment: Qt.AlignCenter
            Layout.topMargin: 5
            text: "Zoom"
            font.pixelSize: 20
        }

        RowLayout {
            spacing: 0
            Layout.leftMargin: 5

            Button {
                id: idBtAScanZoomOut
                Layout.preferredHeight: 30
                Layout.preferredWidth: 30
                background: Rectangle {
                    color: (idBtAScanZoomOut.pressed) ? Design.colButtonSelectedBackground : idBtAScanZoomOut.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                    border.width: 1
                    border.color: Design.colFrameBorder
                }

                text: "-"
                font.pointSize: 12

                onPressed: {
                    scanManager.ascanZoomOut()
                }
            }

            Button {
                id: idBtAScanZoomIn
                Layout.preferredHeight: 30
                Layout.preferredWidth: 30
                background: Rectangle {
                    color: (idBtAScanZoomIn.pressed) ? Design.colButtonSelectedBackground : idBtAScanZoomIn.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                    border.width: 1
                    border.color: Design.colFrameBorder
                }

                text: "+"
                font.pointSize: 12

                onPressed: {
                    scanManager.ascanZoomIn()
                }
            }
        }

        Item {
            // Spacer
            Layout.fillHeight: true
        }
    }

}


import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0
import Fonts 1.0
import "../rp_components"

Rectangle {
    color: "#D5DEE3"
    border.color: "#000000"

    ColumnLayout {
        id: idColAScanCtrl
        anchors.fill: parent

        Text {
            Layout.alignment: Qt.AlignCenter
            text: qsTr("Gain")
            font.family: Fonts.robotoCondensed
            font.pixelSize: 16
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
            implicitHeight: 64
            implicitWidth: 60

            contentItem: TextInput {
                text: sbGain.textFromValue(sbGain.value, sbGain.locale)

                anchors.fill: parent
                anchors.bottomMargin: 30


                font.pixelSize: 16
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
                color: "white"
                border.color: enabled ? Design.colFrameBorder : Design.colButtonHoveredBackground
                border.width: 2
                Text {
                    font.family: Fonts.robotoCondensed
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
                color: "white"
                border.color: enabled ? Design.colFrameBorder : Design.colButtonHoveredBackground
                border.width: 2
                Text {
                    font.family: Fonts.robotoCondensed
                    text: '-'
                    font.pointSize: 12
                    anchors.centerIn: parent
                    color : parent.enabled ? Design.colButtonSelectedText : Design.colButtonSelectedBackground
                }
            }

        }

        Rectangle {
            // Separator
            Layout.leftMargin: 5
            Layout.rightMargin: 5
            Layout.topMargin: 5
            //Layout.bottomMargin: 5
            Layout.fillWidth: true
            Layout.preferredHeight: 2
            color : Design.colSeparator
        }

        Text {
            Layout.alignment: Qt.AlignCenter
            text: qsTr("Résol. x")
            font.family: Fonts.robotoCondensed
            font.pixelSize: 16
        }

        RowLayout {
            spacing: 0
            Layout.leftMargin: 5

            Button {
                id: idBtAScanZoomOut
                Layout.preferredHeight: 30
                Layout.preferredWidth: 30
                background: Rectangle {
                    color: "white"
                    border.width: 2
                    border.color: Design.colFrameBorder
                }

                Text {
                    font.family: Fonts.robotoCondensed
                    text: '-'
                    font.pointSize: 12
                    anchors.centerIn: parent
                    color : parent.enabled ? Design.colButtonSelectedText : Design.colButtonSelectedBackground
                }

                onPressed: {
                    scanManager.ascanZoomOut()
                }
            }

            Button {
                id: idBtAScanZoomIn
                Layout.preferredHeight: 30
                Layout.preferredWidth: 30
                background: Rectangle {
                    color: "white"
                    border.width: 2
                    border.color: Design.colFrameBorder
                }

                Text {
                    font.family: Fonts.robotoCondensed
                    text: '+'
                    font.pointSize: 12
                    anchors.centerIn: parent
                    color : parent.enabled ? Design.colButtonSelectedText : Design.colButtonSelectedBackground
                }

                onPressed: {
                    scanManager.ascanZoomIn()
                }
            }
        }

        Rectangle {
            // Separator
            Layout.leftMargin: 5
            Layout.rightMargin: 5
            Layout.topMargin: 5
            //Layout.bottomMargin: 5
            Layout.fillWidth: true
            Layout.preferredHeight: 2
            color : Design.colSeparator
        }

        Text {
            Layout.alignment: Qt.AlignCenter
            text: qsTr("2nd Echo")
            font.family: Fonts.robotoCondensed
            font.pixelSize: 16
            Layout.topMargin: 5
        }

        SpinBox {
            id: sb2EchoStart

            from: 0
            to: 80
            stepSize: 1
            editable: true

            value: scanManager.SelectedFuncAScan.paramLGate1Start

            onValueChanged: {
                scanManager.SelectedFuncAScan.paramLGate1Start = value
            }
            onValueModified: {
                focus = false;
            }

            Layout.alignment: Qt.AlignCenter
            implicitHeight: 64
            implicitWidth: 60

            contentItem: TextInput {
                text: sb2EchoStart.textFromValue(sb2EchoStart.value, sb2EchoStart.locale)

                anchors.fill: parent
                anchors.bottomMargin: 30

                font.pixelSize: 16
                color: "#000000"

                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter

                readOnly: !sb2EchoStart.editable
                validator: sb2EchoStart.validator
                inputMethodHints: Qt.ImhFormattedNumbersOnly

            }

            up.indicator: Rectangle {
                height: parent.height/2
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                implicitHeight: 30
                implicitWidth: 30 // Adjust width here
                color: "white"
                border.color: enabled ? Design.colFrameBorder : Design.colButtonHoveredBackground
                border.width: 2
                Text {
                    text: '+'
                    font.family: Fonts.robotoCondensed
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
                color: "white"
                border.color: enabled ? Design.colFrameBorder : Design.colButtonHoveredBackground
                border.width: 2
                Text {
                    text: '-'
                    font.family: Fonts.robotoCondensed
                    font.pointSize: 12
                    anchors.centerIn: parent
                    color : parent.enabled ? Design.colButtonSelectedText : Design.colButtonSelectedBackground
                }
            }

        }

        Item {
            // Spacer
            Layout.fillHeight: true
        }
    }

}


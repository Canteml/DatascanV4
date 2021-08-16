import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0

Rectangle {
    id: idBannerVectorInfo
    height: 60
    Layout.fillWidth: true
    Layout.fillHeight: true

    border.width: 1
    border.color: "black"

    property alias idBtPositionPlus: btPositionPlus
    property alias idBtPositionMinus: btPositionMinus
    property alias idLePosition: lePosition
    property alias btVectorConnection: btVectorConnection

    RowLayout {
        id: rowBannerVectorInfo
        anchors.fill: parent
        anchors.leftMargin: 5

        Button {
            id: btScanStartStop

            Layout.preferredWidth: idBannerVectorInfo.height - 10
            Layout.preferredHeight: idBannerVectorInfo.height - 10

            icon.source: "qrc:///img/icons/btStart.png"
            icon.width: idBannerVectorInfo.height - 10
            icon.height: idBannerVectorInfo.height - 10
            icon.color: "transparent"

            background: Rectangle {
                color: btScanStartStop.pressed ? Design.colButtonSelectedBackground : btScanStartStop.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                border.width: 1
                border.color: Design.colButtonBorder
                radius: 10
            }
        }

        Button {
            id: btScanPause

            Layout.preferredWidth: idBannerVectorInfo.height - 10
            Layout.preferredHeight: idBannerVectorInfo.height - 10

            icon.source: "qrc:///img/icons/btPause.png"
            icon.width: idBannerVectorInfo.height - 10
            icon.height: idBannerVectorInfo.height - 10
            icon.color: "transparent"

            background: Rectangle {
                color: btScanPause.pressed ? Design.colButtonSelectedBackground : btScanPause.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                border.width: 1
                border.color: Design.colButtonBorder
                radius: 10
            }
        }

        Button {
            id: btVectorDirection

            Layout.preferredWidth: idBannerVectorInfo.height
            Layout.preferredHeight: idBannerVectorInfo.height

            icon.source: "qrc:///img/icons/altiscan_up_down.png"
            icon.width: idBannerVectorInfo.height
            icon.height: idBannerVectorInfo.height
            icon.color: "transparent"
            padding: 1

            background: Rectangle {
                color: "transparent"
            }
        }

        Rectangle {
            // seaparator
            width: 5
            Layout.fillHeight: true
            color: "#9C9B9B"
            Layout.topMargin: 5
            Layout.bottomMargin: 5
        }

        // -------------------
        // -- Position
        Image {
            id: icPosition
            Layout.fillHeight: true
            Layout.topMargin: 5
            Layout.bottomMargin: 5
            Layout.preferredWidth: 40
            source: "qrc:///img/icons/iconPosition.png"
            fillMode: Image.PreserveAspectFit
        }

        RowLayout {

            //Layout.fillHeight: true
            TextInput {
                id: lePosition
                Layout.minimumWidth: 120

                //Layout.fillHeight: true
                text: "12.34"
                font.family: "Helvetica"
                font.pixelSize: 48
                horizontalAlignment: Text.AlignRight
                color: "black"
                maximumLength: 5
                smooth: true
                validator: RegExpValidator {
                    regExp: /^(?:[1-9]\d*|0)?(?:\.\d+)?$/
                }

                onAccepted: focus = false

                //readOnly: isReadOnly
            }

            Item {
                id: itPosition
                Layout.fillHeight: true
                Layout.preferredWidth: 40

                Label {
                    id: lbPositionDim
                    anchors.fill: itPosition
                    anchors.bottomMargin: 10
                    text: "m"
                    font.pixelSize: 36
                    verticalAlignment: Text.AlignBottom
                }

                ColumnLayout {
                    anchors.fill: itPosition
                    anchors.topMargin: 5
                    anchors.bottomMargin: 5
                    spacing: 0

                    visible: lePosition.focus | btPositionPlus.focus | btPositionMinus.focus

                    Button {
                        id: btPositionPlus
                        text: "+"
                        font.pixelSize: 24
                        //Layout.preferredWidth: lbPositionDim.width
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        background: Rectangle {
                            color: btPositionPlus.pressed ? Design.colButtonSelectedBackground : btPositionPlus.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                            border.width: 1
                            border.color: "black"
                        }

                        onPressed: lePosition.focus = true

                        //on: focus = false
                    }
                    Button {
                        id: btPositionMinus
                        text: "-"
                        font.pixelSize: 24
                        //Layout.preferredWidth: lbPositionDim.width
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        background: Rectangle {
                            color: btPositionMinus.pressed ? Design.colButtonSelectedBackground : btPositionMinus.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                            border.width: 1
                            border.color: "black"
                        }

                        onPressed: lePosition.focus = true
                    }
                }
            }
        }

        Item {
            id: itSpacerHorizontal
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Rectangle {
            // seaparator
            width: 5
            Layout.fillHeight: true
            color: Design.colSeparator
            Layout.topMargin: 5
            Layout.bottomMargin: 5
        }

        // -------------------
        // Peripherals
        Image {
            id: icJoystick
            Layout.fillHeight: true
            Layout.topMargin: 5
            Layout.bottomMargin: 5
            source: "qrc:///img/icons/iconJoystickDisconnected.png"
            fillMode: Image.PreserveAspectFit
        }

        Rectangle {
            // seaparator
            width: 5
            Layout.fillHeight: true
            color: Design.colSeparator
            Layout.topMargin: 5
            Layout.bottomMargin: 5
        }

        // -------------------
        // Connections
        Button {
            id: btVectorConnection

            Layout.preferredWidth: idBannerVectorInfo.height
            Layout.preferredHeight: idBannerVectorInfo.height

            icon.source: "qrc:///img/icons/iconDisconnected.png"
            icon.width: idBannerVectorInfo.height
            icon.height: idBannerVectorInfo.height
            icon.color: "transparent"
            padding: 1

            background: Rectangle {
                color: "transparent"
            }
        }

        Button {
            id: btVectorEmergencyStop

            Layout.preferredWidth: idBannerVectorInfo.height
            Layout.preferredHeight: idBannerVectorInfo.height

            icon.source: "qrc:///img/icons/btEmergencyStop.png"
            icon.width: idBannerVectorInfo.height
            icon.height: idBannerVectorInfo.height
            icon.color: "transparent"
            padding: 1

            background: Rectangle {
                color: "transparent"
            }
        }
    }
}

import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0
import "../rp_components"

Rectangle {

    color: "#FFFFFF"
    border.width: 1
    //Layout.preferredHeight: gridLayout.height

    ColumnLayout {

        anchors.fill: parent

        // -------------------
        // -- TOP BUTTONS
        Rectangle {
            id: idSettingsAScanBar

            height: 35
            Layout.fillWidth: true

            RowLayout {
                id: idRowSettingsAScan
                anchors.fill: parent
                spacing: 0

                Button {
                    id: idBtSensor
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    background: Rectangle {
                        color: (idBtSensor.pressed
                                || (idSlSettingsAScan.visible
                                    && idSlSettingsAScan.currentIndex
                                    == 0)) ? Design.colButtonSelectedBackground : idBtSensor.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        //color: btBannerTitle.hovered ? "#9C9B9B" : "#FFFFFF"
                        border.width: 1
                        border.color: Design.colFrameBorder
                    }

                    contentItem: Text {
                        text: qsTr("CAPTEUR")
                        font.pointSize: 12
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onPressed: {
                        idSlSettingsAScan.currentIndex = 0;
                    }
                }

                Button {
                    id: idBtGate
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    background: Rectangle {
                        color: (idBtGate.pressed
                                || (idSlSettingsAScan.visible
                                    && idSlSettingsAScan.currentIndex
                                    == 1)) ? Design.colButtonSelectedBackground : idBtGate.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                        //color: btBannerTitle.hovered ? "#9C9B9B" : "#FFFFFF"
                        border.width: 1
                        border.color: Design.colFrameBorder
                    }

                    contentItem: Text {
                        text: qsTr("PORTES")
                        font.pointSize: 12
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    onPressed: {
                        idSlSettingsAScan.currentIndex = 1;
                    }
                }
            }
        }

        StackLayout {
            id: idSlSettingsAScan
            currentIndex: 0
            Layout.leftMargin: 10

            // -- SETTINGS SENSOR
            GridLayout {
                id: gridLayout
                columns: 2

                // --
                Text {
                    text: "Gain (dB) :"
                    font.pointSize: 12
                }

                RpSpinBox {
                    id: idSbGain
                    from: 0
                    to: 80
                    stepSize: 1
                    editable: true
                    font.pointSize: 12

                    value: scanManager.SelectedFuncAScan.paramDGain

                    onValueChanged: {
                        scanManager.SelectedFuncAScan.paramDGain = value
                    }
                    onValueModified: {
                        focus = false;
                    }

    //                id: idSbGain
    //                from: 0
    //                to: 80*100
    //                stepSize: 100
    //                editable: true
    //                font.pointSize: 12

    //                validator: DoubleValidator {
    //                    bottom: Math.min(idSbGain.from, idSbGain.to)
    //                    top:  Math.max(idSbGain.from, idSbGain.to)
    //                }

    //                textFromValue: function(value, locale) {
    //                    return Number(value / 100).toLocaleString(locale, 'f', 2)
    //                }

    //                valueFromText: function(text, locale) {
    //                    return Number.fromLocaleString(locale, text) * 100
    //                }

    //                onValueChanged: {
    //                    currentVector.funcAScan0().paramDGain = value/100
    //                }

    //                Connections {
    //                    target: currentVector.funcAScan0()
    //                    function onParamDGainChanged() {
    //                        idSbGain.value = currentVector.funcAScan0().paramDGain*100
    //                    }
    //                }

                }

                //--
                Text {
                    text: qsTr("Voltage (V) :")
                    font.pointSize: 12
                }

                RpSpinBox {
                    id: idSbVoltage
                    from: 10
                    to: 230
                    stepSize: 1
                    editable: true
                    font.pointSize: 12

                    value: scanManager.SelectedFuncAScan.paramDVoltage

                    onValueChanged: {
                        scanManager.SelectedFuncAScan.paramDVoltage = value
                    }

                    onValueModified: {
                        focus = false;
                    }
                }

                // --
                Text {
                    text: qsTr("Element :")
                    font.pointSize: 12
                }

                RpSpinBox {
                    id: idSbUtElement
                    from: 1
                    to: 2
                    stepSize: 1
                    editable: true
                    font.pointSize: 12

                    value: scanManager.SelectedFuncAScan.paramDUtElement

                    onValueChanged: {
                        scanManager.SelectedFuncAScan.paramDUtElement = value
                    }
                    onValueModified: {
                        focus = false;
                    }
                }

                // --
                Text {
                    text: qsTr("Fréquence éch. (MHz) :")
                    font.pointSize: 12
                }

                RpSpinBox {
                    id: idSbFreq
                    from: 0
                    to: items.length - 1

                    stepSize: 1
                    editable: true
                    font.pointSize: 12

                    property var items: ["20", "40", "80"]

                    validator: RegExpValidator {
                        regExp: new RegExp("(20|40|80)", "i")
                    }

                    textFromValue: function(value) {
                        return items[value];
                    }

                    valueFromText: function(text) {
                        for (var i = 0; i < items.length; ++i) {
                            if (items[i].toLowerCase().indexOf(text.toLowerCase()) === 0)
                                return i
                        }
                        return sb.value
                    }

                    value: 2-scanManager.SelectedFuncAScan.paramDSampleFreq

                    onValueChanged: {
                        scanManager.SelectedFuncAScan.paramDSampleFreq = 2-value
                    }
                    onValueModified: {
                        focus = false;
                    }
                }

                // --
                Text {
                    text: qsTr("Largeur impulsion (ns) :")
                    font.pointSize: 12
                }

                RpSpinBox {
                    id: idSbPulseWidth
                    from: 18
                    to: 1600

                    stepSize: 1
                    editable: true
                    font.pointSize: 12

                    value: scanManager.SelectedFuncAScan.paramDPulseWidth

                    onValueChanged: {
                        scanManager.SelectedFuncAScan.paramDPulseWidth = value
                    }
                    onValueModified: {
                        focus = false;
                    }
                }

                // --
                Text {
                    text: qsTr("Vitesse US (m/s) :")
                    font.pointSize: 12
                }

                RpSpinBox {
                    id: idSbCelerity
                    from: 1000
                    to: 10000

                    stepSize: 1
                    editable: true
                    font.pointSize: 12

                    value: scanManager.SelectedFuncAScan.paramLCelerity

                    onValueChanged: {
                        scanManager.SelectedFuncAScan.paramLCelerity = value
                    }
                    onValueModified: {
                        focus = false;
                    }
                }

                // --
                Text {
                    text: qsTr("Filtrage :")
                    font.pointSize: 12
                }

                RpSpinBox {
                    id: idSbFiltre
                    from: 0
                    to: 50

                    stepSize: 1
                    editable: true
                    font.pointSize: 12

                    value: scanManager.SelectedFuncAScan.paramLLowPassFilter

                    onValueChanged: {
                        scanManager.SelectedFuncAScan.paramLLowPassFilter = value
                    }
                    onValueModified: {
                        focus = false;
                    }
                }
            }

            // -- SETTINGS GATES
            ColumnLayout {
                RowLayout {
                    spacing: 15
                    // GATE 0
                    ColumnLayout {
                        Text {
                            text: qsTr("Porte 1er écho :")
                            color: Design.colRed
                            font.pointSize: 12
                        }

                        GridLayout {
                            columns: 2

                            Image {
                                Layout.maximumHeight: 40
                                Layout.maximumWidth: 60
                                source: "qrc:///img/icons/iconGateRedStart.png"
                                fillMode: Image.PreserveAspectFit
                            }

                            RpSpinBox {
                                id: idGate0Start
                                from: 1
                                to: 10000

                                stepSize: 1
                                editable: true
                                font.pointSize: 12

                                value : scanManager.SelectedFuncAScan.paramLGate0Start
                                onValueChanged: {
                                    if( value !== scanManager.SelectedFuncAScan.paramLGate0Start ) {
                                        console.log("paramLGate0Start last value : " + scanManager.SelectedFuncAScan.paramLGate0Start + " / new : " + value )
                                        scanManager.SelectedFuncAScan.paramLGate0Start = value;
                                    }
                                }
                                onValueModified: {
                                    focus = false;
                                }
                            }

                            Image {
                                Layout.maximumHeight: 40
                                Layout.maximumWidth: 60
                                source: "qrc:///img/icons/iconGateRedWidth.png"
                                fillMode: Image.PreserveAspectFit
                            }

                            RpSpinBox {
                                id: idGate0Width
                                from: 1
                                to: 10000

                                stepSize: 1
                                editable: true
                                font.pointSize: 12

                                value : scanManager.SelectedFuncAScan.paramLGate0Width
                                onValueChanged: {
                                    if( value !== scanManager.SelectedFuncAScan.paramLGate0Width ) {
                                        console.log("paramLGate0Width last value : " + scanManager.SelectedFuncAScan.paramLGate0Width + " / new : " + value )
                                        scanManager.SelectedFuncAScan.paramLGate0Width = value;
                                    }
                                }
                                onValueModified: {
                                    focus = false;
                                }
                            }

                            Image {
                                Layout.maximumHeight: 40
                                Layout.maximumWidth: 60
                                source: "qrc:///img/icons/iconGateRedHeight.png"
                                fillMode: Image.PreserveAspectFit
                            }

                            RpSpinBox {
                                id: idGate0Height
                                from: 1
                                to: 10000

                                stepSize: 1
                                editable: true
                                font.pointSize: 12

                                value : scanManager.SelectedFuncAScan.paramLGate0Height
                                onValueChanged: {
                                    if( value !== scanManager.SelectedFuncAScan.paramLGate0Height ) {
                                        console.log("paramLGate0Height last value : " + scanManager.SelectedFuncAScan.paramLGate0Height + " / new : " + value )
                                        scanManager.SelectedFuncAScan.paramLGate0Height = value;
                                    }
                                }
                                onValueModified: {
                                    focus = false;
                                }
                            }
                        }

                    }

                    // GATE 2
                    ColumnLayout {
                        Text {
                            text: qsTr("Porte 2nd écho :")
                            color: Design.colGreen
                            font.pointSize: 12
                        }

                        GridLayout {
                            columns: 2

                            Image {
                                Layout.maximumHeight: 40
                                Layout.maximumWidth: 60
                                source: "qrc:///img/icons/iconGateGreenStart.png"
                                fillMode: Image.PreserveAspectFit
                            }

                            RpSpinBox {
                                id: idGate1Start
                                from: 1
                                to: 1000

                                stepSize: 1
                                editable: true
                                font.pointSize: 12

                                value : scanManager.SelectedFuncAScan.paramLGate1Start
                                onValueChanged: {
                                    if( value !== scanManager.SelectedFuncAScan.paramLGate1Start ) {
                                        console.log("paramLGate1Startlast value : " + scanManager.SelectedFuncAScan.paramLGate1Start + " / new : " + value )
                                        scanManager.SelectedFuncAScan.paramLGate1Start = value;
                                    }
                                }
                                onValueModified: {
                                    focus = false;
                                }
                            }

                            Image {
                                Layout.maximumHeight: 40
                                Layout.maximumWidth: 60
                                source: "qrc:///img/icons/iconGateGreenWidth.png"
                                fillMode: Image.PreserveAspectFit
                            }

                            RpSpinBox {
                                id: idGate1Width
                                from: 1
                                to: 1000

                                stepSize: 1
                                editable: true
                                font.pointSize: 12

                                value : scanManager.SelectedFuncAScan.paramLGate1Width
                                onValueChanged: {
                                    if( value !== scanManager.SelectedFuncAScan.paramLGate1Width ) {
                                        console.log("paramLGate1Width last value : " + scanManager.SelectedFuncAScan.paramLGate1Width + " / new : " + value )
                                        scanManager.SelectedFuncAScan.paramLGate1Width = value;
                                    }
                                }
                                onValueModified: {
                                    focus = false;
                                }

                            }

                            Image {
                                Layout.maximumHeight: 40
                                Layout.maximumWidth: 60
                                source: "qrc:///img/icons/iconGateGreenHeight.png"
                                fillMode: Image.PreserveAspectFit
                            }

                            RpSpinBox {
                                id: idGate1Height
                                from: 1
                                to: 100

                                stepSize: 1
                                editable: true
                                font.pointSize: 12

                                value : scanManager.SelectedFuncAScan.paramLGate1Height
                                onValueChanged: {
                                    if( value !== scanManager.SelectedFuncAScan.paramLGate1Height ) {
                                        console.log("paramLGate1Height last value : " + scanManager.SelectedFuncAScan.paramLGate1Height + " / new : " + value )
                                        scanManager.SelectedFuncAScan.paramLGate1Height = value;
                                    }
                                }
                                onValueModified: {
                                    focus = false;
                                }
                            }
                        }

                    }
                }
                RowLayout {
                    // --
                    Text {
                        text: qsTr("Retard palpeur (mm) :")
                        font.pointSize: 12
                    }

                    RpSpinBox {
                        id: idSbProbeDelay
                        from: 0
                        to: 1000*10
                        implicitWidth: 120

                        stepSize: 1
                        editable: true
                        font.pointSize: 12

//                        value : scanManager.SelectedFuncAScan.paramLProbeDelayMm
//                        onValueChanged: {
//                            scanManager.SelectedFuncAScan.paramLProbeDelayMm = value
//                        }
                        onValueModified: {
                            focus = false;
                        }

//                        onValueChanged: {
//                            currentVector.funcAScan0().paramDGain = value
//                        }
//                        onValueModified: {
//                            focus = false;
//                        }


                        validator: DoubleValidator {
                            bottom: Math.min(idSbProbeDelay.from, idSbProbeDelay.to)
                            top:  Math.max(idSbProbeDelay.from, idSbProbeDelay.to)
                        }

                        textFromValue: function(value, locale) {
                            return Number(value / 10).toLocaleString(locale, 'f', 1)
                        }

                        valueFromText: function(text, locale) {
                            return Number.fromLocaleString(locale, text) * 10
                        }

                        onValueChanged: {
                            scanManager.SelectedFuncAScan.paramLProbeDelayMm = value/10
                        }

                        Connections {
                            target: scanManager.SelectedFuncAScan
                            function onParamLProbeDelayMmChanged() {
                                updateValueDisplayed();
                            }
                        }

                        Component.onCompleted: {
                            updateValueDisplayed();
                        }

                        function updateValueDisplayed() {
                            idSbProbeDelay.value = scanManager.SelectedFuncAScan.paramLProbeDelayMm*10
                        }

        //                id: idSbGain
        //                from: 0
        //                to: 80*100
        //                stepSize: 100
        //                editable: true
        //                font.pointSize: 12

        //                validator: DoubleValidator {
        //                    bottom: Math.min(idSbGain.from, idSbGain.to)
        //                    top:  Math.max(idSbGain.from, idSbGain.to)
        //                }

        //                textFromValue: function(value, locale) {
        //                    return Number(value / 100).toLocaleString(locale, 'f', 2)
        //                }

        //                valueFromText: function(text, locale) {
        //                    return Number.fromLocaleString(locale, text) * 100
        //                }

        //                onValueChanged: {
        //                    currentVector.funcAScan0().paramDGain = value/100
        //                }

        //                Connections {
        //                    target: currentVector.funcAScan0()
        //                    function onParamDGainChanged() {
        //                        idSbGain.value = currentVector.funcAScan0().paramDGain*100
        //                    }
        //                }

                    }
                }
            }
        }
    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

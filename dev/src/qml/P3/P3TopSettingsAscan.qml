import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

Rectangle {

    color: "#AABB00"
    Layout.preferredHeight: gridLayout.height

    Item {
        anchors.fill: parent
        anchors.margins: 5

        GridLayout {
            id: gridLayout
            columns: 2
            Layout.topMargin: 50
            Layout.leftMargin: 50

            // --
            Text {
                text: "Gain (dB) :"
                font.pointSize: 12
            }

            SpinBox {
                id: idSbGain
                from: 0
                to: 80
                stepSize: 1
                editable: true
                font.pointSize: 12

                value: currentVector.funcAScan0().paramDGain

                onValueChanged: {
                    currentVector.funcAScan0().paramDGain = value
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

            SpinBox {
                id: idSbVoltage
                from: 10
                to: 230
                stepSize: 1
                editable: true
                font.pointSize: 12

                value: currentVector.funcAScan0().paramDVoltage

                onValueChanged: {
                    currentVector.funcAScan0().paramDVoltage = value
                }
            }

            // --
            Text {
                text: qsTr("Element :")
                font.pointSize: 12
            }

            SpinBox {
                id: idSbUtElement
                from: 1
                to: 2
                stepSize: 1
                editable: true
                font.pointSize: 12

                value: currentVector.funcAScan0().paramDUtElement

                onValueChanged: {
                    currentVector.funcAScan0().paramDUtElement = value
                }
            }

            // --
            Text {
                text: qsTr("Fréquence éch. (MHz) :")
                font.pointSize: 12
            }

            SpinBox {
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

                value: 2-currentVector.funcAScan0().paramDSampleFreq

                onValueChanged: {
                    currentVector.funcAScan0().paramDSampleFreq = 2-value
                }
            }

            // --
            Text {
                text: qsTr("Largeur impulsion (ns) :")
                font.pointSize: 12
            }

            SpinBox {
                id: idSbPulseWidth
                from: 18
                to: 18+255*6.25

                stepSize: 1
                editable: true
                font.pointSize: 12

                value: 18+currentVector.funcAScan0().paramDPulseWidth*6.25

                onValueChanged: {
                    currentVector.funcAScan0().paramDPulseWidth = (value-18)/6.25
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

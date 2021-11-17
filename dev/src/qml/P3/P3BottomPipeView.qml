import QtQuick 2.15
import QtMultimedia 5.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0

import "../rp_components"

Rectangle {

    RowLayout {
        anchors.fill: parent
        spacing: 0
        PipeSensor {
            sensorNumber : 0
            selectShorcut1 : "1"
            selectShorcut2 : "&"
        }
        PipeSensor {
            sensorNumber : 1
            selectShorcut1 : "2"
            selectShorcut2 : "é"
        }
        PipeSensor {
            sensorNumber : 2
            selectShorcut1 : "3"
            selectShorcut2 : "\""
        }
        PipeSensor {
            sensorNumber : 3
            selectShorcut1 : "4"
            selectShorcut2 : "'"
        }
    }
}

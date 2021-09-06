import QtQuick 2.15
import QtQuick.Layouts 1.11
import Design 1.0

/*
  Text + line to make section in forms
*/
RowLayout {
    Layout.fillWidth: true

    property alias text: idLb.text

    Text {
        id: idLb
        text: qsTr("Titre")
        font.pixelSize: 18
        font.bold: true
        color: Design.colFromSection
    }

    Rectangle {
        id: idLineEq
        Layout.fillWidth: true
        Layout.minimumHeight: 3
        color: Design.colFromSection
    }
}


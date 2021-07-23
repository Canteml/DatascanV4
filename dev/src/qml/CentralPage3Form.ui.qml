import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import "./P3"

Rectangle {
    id: idCentralP3

    Layout.fillWidth: true
    Layout.fillHeight: true

    border.width: 0

    ColumnLayout {
        id: colP3
        anchors.fill: parent

        spacing: 0

        P3Top {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        P3Bottom {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}

import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

Rectangle {

    color: "#005580"
    Layout.preferredWidth: 70

    RowLayout {

        anchors.fill: parent
        spacing: 0

        P3TopAscanCtrl {
            Layout.preferredWidth: 70
            Layout.fillHeight: true
        }
    }
}

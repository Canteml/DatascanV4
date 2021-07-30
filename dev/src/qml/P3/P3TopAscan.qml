import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

import AScanGraph 1.0

Rectangle {

    color: "#005580"
    Layout.preferredWidth: 70

    RowLayout {
        anchors.fill: parent
        spacing: 0

        P3TopAscanCtrl {
            objectName: "TestObj"
            Layout.preferredWidth: 70
            Layout.fillHeight: true
        }

        AScanGraph {
            objectName: "AScanGraphMain"
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

Rectangle {
    id: idP3Top

    border.width: 1
    border.color: "black"

    RowLayout {
        id: idP3TopRow
        anchors.fill: parent

        spacing: 0

        P3TopAscan {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        P3TopCamSettings {
            // Layout.fillWidth: true => controled by P3TopCamSettings
            Layout.fillHeight: true
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

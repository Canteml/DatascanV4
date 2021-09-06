import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11

RowLayout {

    property alias idBtPrev: idBtPrev
    property alias idBtNext: idBtNext

    Layout.preferredHeight: 40
    Layout.fillHeight: true
    Layout.margins: 5

    Button {
        id: idBtPrev
        Layout.preferredHeight: 40
        Layout.preferredWidth: 550/(153/Layout.preferredHeight) // icon size = 550*153

        text: "XXX"
        font.pixelSize: 20

        background: Image {
            id: idImBtPrev
            source: idBtPrev.pressed ? "qrc:///img/icons/btArrow_select.png" : idBtPrev.hovered ? "qrc:///img/icons/btArrow_hover.png" : "qrc:///img/icons/btArrow_std.png"
            mirror: true
        }
    }

    Item {
        // Spacer
        Layout.fillWidth: true
    }

    Button {
        id: idBtNext
        Layout.preferredHeight: 40
        Layout.preferredWidth: 550/(153/Layout.preferredHeight) // icon size = 550*153

        text: "XXX"
        font.pixelSize: 20

        background: Image {
            id: idImBtNext
            source: idBtNext.pressed ? "qrc:///img/icons/btArrow_select.png" : idBtNext.hovered ? "qrc:///img/icons/btArrow_hover.png" : "qrc:///img/icons/btArrow_std.png"
        }
    }
}


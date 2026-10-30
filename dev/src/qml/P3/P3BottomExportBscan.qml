import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0
import Fonts 1.0

import BScanGraph 1.0

// -------------------
// -- EXPORT BSCAN (bottom) : shows the whole recorded BSCAN of the exam being exported.
// The BScanGraph "BScanGraphExport" is filled by ScanManager when examStep === EXAM_EXPORTING
// (selected location, VIEW_X_FULL / VIEW_Y_FULL), and cleared at any other step.

Rectangle {
    id: idP3BottomExportBscan

    color: "white"
    border.width: 1
    border.color: "#000000"

    BScanGraph {
        objectName: "BScanGraphExport"
        anchors.fill: parent
        anchors.margins: 1
        enabled: false      // display only : no zoom / drag
    }
}

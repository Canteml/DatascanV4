import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Window  {
    objectName: "mainWindow"
    visible: true
    width: 640
    height: 480
    title: qsTr("DATASCAN")

    flags: Qt.Window

    // Trick to get focus wherever the mouse click, and remove focus from last item

    Item {
        focus: true
        anchors.fill: parent
        id: idMaster
        MouseArea {
            anchors.fill: parent
            onClicked: focus = true
        }

        Image {
            id: image1
            height: 270
            anchors.top: parent.top
            anchors.topMargin: 50
            anchors.right: parent.right
            anchors.rightMargin: 100
            anchors.left: parent.left
            anchors.leftMargin: 100
            source: "qrc:///img/logo.png"
        }

        StackLayout {
            id: idWindowStackLayout

            anchors.fill: parent

            currentIndex: 0

            Mainwindow {
                Component.onCompleted: focus = true
            }

            Rectangle {
                id: idAlert4Gas
                Layout.preferredHeight: 70
                Layout.fillWidth: true
                color: "red"
                border.width: 1
                border.color: "black"

                Text {
                    id: idText4Gas
                    text: "4-GAS ALERT DETECTED!"
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    anchors.fill: parent
                    wrapMode: Text.WordWrap
                    font.pixelSize: 35
                    color: "white"
                }
            }

        }

        // -- 4-GAS blocking
        Connections {
            target: currentVector
            function onAlertChanged(alertLevel) {
                if(alertLevel === 2 && currentVector.getAlertMsg(alertLevel) === "4-GAS ALERT DETECTED!") {
                    idWindowStackLayout.currentIndex = 1
                } else {
                    idWindowStackLayout.currentIndex = 0
                }
            }
        }

        // -- KEYBOARD ACTIONS
        property var actMoveUp : false;
        property var actMoveDown : false;
        property var actMoveLeft : false;
        property var actMoveRight : false;
        property var actMoveBoost : false

        Keys.onPressed: {
            if( !event.isAutoRepeat ) {
                if( globalManager.keyboardQwerty ) {
                    if (event.key === Qt.Key_W ) actMoveUp = true;
                    if (event.key === Qt.Key_S ) actMoveDown = true;
                    if (event.key === Qt.Key_A ) actMoveLeft = true;
                    if (event.key === Qt.Key_D ) actMoveRight = true;
                } else {
                    if (event.key === Qt.Key_Z ) actMoveUp = true;
                    if (event.key === Qt.Key_S ) actMoveDown = true;
                    if (event.key === Qt.Key_Q ) actMoveLeft = true;
                    if (event.key === Qt.Key_D ) actMoveRight = true;
                }

                if (event.key === Qt.Key_Shift ) {
                    actMoveBoost = true;
                    event.accepted = false;
                }

            }

            if(event.key === Qt.Key_Up) examManager.mepRow -= 1
            if(event.key === Qt.Key_Down) examManager.mepRow += 1
            if(event.key === Qt.Key_Left) examManager.mepCol -= 1
            if(event.key === Qt.Key_Right) examManager.mepCol += 1

            if(examManager.controlType === 1 &&
                    ( event.key === Qt.Key_Up || event.key === Qt.Key_Down || event.key === Qt.Key_Left || event.key === Qt.Key_Right )
              ) examManager.isMepAutoMode = false

            if( (event.key === Qt.Key_Return) && (examManager.controlType === 1) ) {
                if(currentVector.funcAScan0().hasReceivedMessage)
                    examManager.startWaitingForMepAScan();
            }

            if( (event.key === Qt.Key_O) && (examManager.controlType === 1) ) {
                if(currentVector.funcAScan0().hasReceivedMessage)
                    examManager.isMepAutoMode = !examManager.isMepAutoMode
            }

        }

        Keys.onReleased: {
            if( !event.isAutoRepeat ) {
                if( globalManager.keyboardQwerty ) {
                    if (event.key === Qt.Key_W ) actMoveUp = false;
                    if (event.key === Qt.Key_S ) actMoveDown = false;
                    if (event.key === Qt.Key_A ) actMoveLeft = false;
                    if (event.key === Qt.Key_D ) actMoveRight = false;
                } else {
                    if (event.key === Qt.Key_Z ) actMoveUp = false;
                    if (event.key === Qt.Key_S ) actMoveDown = false;
                    if (event.key === Qt.Key_Q ) actMoveLeft = false;
                    if (event.key === Qt.Key_D ) actMoveRight = false;
                }

                if (event.key === Qt.Key_Shift ) {
                    actMoveBoost = false;
                    event.accepted = false;
                }


            }
        }

        onActMoveUpChanged: {
            currentVector.funcMove().moveKeyUpState(actMoveUp);
        }

        onActMoveDownChanged: {
            currentVector.funcMove().moveKeyDownState(actMoveDown);
        }

        onActMoveLeftChanged: {
            currentVector.funcMove().moveKeyLeftState(actMoveLeft);
        }

        onActMoveRightChanged: {
            currentVector.funcMove().moveKeyRightState(actMoveRight);
        }

        onActMoveBoostChanged: {
            currentVector.funcMove().moveBoost(actMoveBoost)
        }
    }

    onActiveChanged: {
        if (!active) {
            currentVector.funcMove().stopMove();
            idMaster.actMoveDown = false;
            idMaster.actMoveLeft = false;
            idMaster.actMoveUp = false;
            idMaster.actMoveRight = false;
            idMaster.actMoveBoost = false;
        }
    }
}

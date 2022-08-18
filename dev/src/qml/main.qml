import QtQuick 2.7
import QtQuick.Controls 2.15
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2

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

        Mainwindow {
            Component.onCompleted: focus = true
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

                if(event.key === Qt.Key_Up) {

                }
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

                if( (event.key === Qt.Key_Return) && (examManager.controlType === 1) ) {
                    if(currentVector.funcAScan0().hasReceivedMessage)
                        examManager.startWaitingForMepAScan();
                }
            }
        }

        onActMoveUpChanged: {
            if( actMoveUp ) {
                currentVector.funcMove().moveKeyUp();
            } else {
                currentVector.funcMove().moveStopY();
            }
        }

        onActMoveDownChanged: {
            if( actMoveDown ) {
                currentVector.funcMove().moveKeyDown();
            } else {
                currentVector.funcMove().moveStopY();
            }
        }

        onActMoveLeftChanged: {
            if( actMoveLeft ) {
                currentVector.funcMove().moveKeyLeft();
            } else {
                currentVector.funcMove().moveStopX();
            }
        }

        onActMoveRightChanged: {
            if( actMoveRight ) {
                currentVector.funcMove().moveKeyRight();
            } else {
                currentVector.funcMove().moveStopX();
            }
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

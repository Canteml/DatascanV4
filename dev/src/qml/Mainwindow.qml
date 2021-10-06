import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0

Item {
    id: itemMain

    anchors.fill: parent
    focus: enabled
//    property alias btBannerTitle: btBannerTitle
//    property alias slCentral: slCentral

//    property alias btMainMenu: btMainMenu
    //property alias colBannerInfo: colBannerInfo
    //    property alias lbBannerScan: lbBannerScan
    //    property alias lbBannerPiece: lbBannerPiece
//    property alias rowVectorInfo: rowVectorInfo
//    property alias rowBanner: rowBanner
//    property alias colBtPage: colBtPage
//    property alias btCentralPage0: btCentralPage0
//    property alias btCentralPage1: btCentralPage1
//    property alias btCentralPage2: btCentralPage2
//    property alias btCentralPage3: btCentralPage3
//    property alias btCentralPage4: btCentralPage4
//    property alias itCentralView: itCentralView
//    property alias colMainLayout: colMainLayout

    readonly property int page0_project : 0;
    readonly property int page1_prepare : 1;
    readonly property int page2_supervise : 2;
    readonly property int page3_scan: 3;
    readonly property int page4_help: 4;

    property var displayMenu: false

    ColumnLayout {
        id: colMainLayout
        anchors.fill: parent
        spacing: 0

        RowLayout {
            id: rowBanner
            Layout.preferredHeight: 70
            Layout.minimumHeight: 70
            Layout.fillHeight: false
            Layout.fillWidth: true
            spacing: 0

            Button {
                id: btMainMenu
                Layout.preferredWidth: rowBanner.height
                Layout.preferredHeight: rowBanner.height
                highlighted: true

                icon.source: "qrc:///img/icons/btMainMenuNormal.png"
                icon.width: rowBanner.height
                icon.height: rowBanner.height
                icon.color: "transparent"

                background: Rectangle {
                    color: Design.colButtonSelectedBackground
                    border.width: 1
                    border.color: "black"
                }

                padding: 0

                onHoveredChanged: checkMainMenuHovered();
            }

            Button {
                id: btBannerTitle
                text: examManager.currentExamName
                font.pointSize: 12
                Layout.preferredWidth: 160
                Layout.fillWidth: false
                Layout.fillHeight: true
                padding: 0

                contentItem: Text {
                    text: btBannerTitle.text
                    font: btBannerTitle.font
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    lineHeight: 1.25 // line-spacing
                }

                background: Rectangle {
                    id: ibBtBannerTitleBG
                    color: examManager.examStep !== 0 /*stopped*/ ?
                               Design.colGreen : Design.colButtonBackground //btBannerTitle.pressed ? Design.colButtonSelectedBackground : btBannerTitle.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                    //color: btBannerTitle.hovered ? "#9C9B9B" : "#FFFFFF"
                    border.width: 1
                    border.color: Design.colFrameBorder

//                    ColorAnimation on color{
//                        from:"#FFFFFFFF"
//                        to:"#F0FF0000"
//                        duration: 500
//                        running:  examManager.examStep === 1 // recording measure
//                        loops : Animation.Infinite
//                        onRunningChanged: if(!running){ibBtBannerTitleBG.color = "#FFFFFFFF"} // reset
//                    }
                }

                onPressed: { globalManager.guiPage = globalManager.guiPage === 3 ? 2 : 3;}
            }

            //            ColumnLayout {
            //                id: colBannerInfo
            //                Layout.preferredWidth: 150
            //                Layout.fillWidth: false
            //                spacing: 0

            //                Label {
            //                    id: lbBannerScan
            //                    text: qsTr("PIECE")
            //                    horizontalAlignment: Text.AlignHCenter
            //                    verticalAlignment: Text.AlignVCenter
            //                    font.pointSize: 10
            //                    Layout.fillHeight: true
            //                    Layout.fillWidth: true
            //                    background: Rectangle {
            //                        color: "#C9C7C7"
            //                    }
            //                }
            //                Label {
            //                    id: lbBannerPiece
            //                    text: qsTr("GENERATRICE")
            //                    horizontalAlignment: Text.AlignHCenter
            //                    verticalAlignment: Text.AlignVCenter
            //                    font.pointSize: 10
            //                    Layout.fillHeight: true
            //                    Layout.fillWidth: true
            //                    background: Rectangle {
            //                        color: "#C9C7C7"
            //                    }
            //                }
            //            }
            RowLayout {
                id: rowVectorInfo
                Layout.fillWidth: true

                BannerVectorInfo {}
            }
        }

        Rectangle {
            id: itCentralView
            Layout.fillWidth: true
            Layout.fillHeight: true

            StackLayout {
                id: slCentral

                anchors.fill: parent

                currentIndex: 0

                CentralPage0 {}
                CentralPage1 {}
                CentralPage2 {}
                CentralPage3 {}
                CentralPage4 {}
            }

            ColumnLayout {
                id: colBtPage
                anchors.left: itCentralView.left
                anchors.top: itCentralView.top
                spacing: 0
                transitions: Transition {
                    NumberAnimation {
                        properties: "height"
                        easing.type: Easing.InOutQuad
                    }
                }

                Button {
                    id: btCentralPage0
                    Layout.preferredWidth: 140
                    Layout.preferredHeight: 60
                    text: qsTr("PROJET")
                    font.pointSize: 12
                    background: Rectangle {
                        color: Design.colButtonDarkBackground
                    }
                    contentItem: Text {
                        text: btCentralPage0.text
                        font: btCentralPage0.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        color: btCentralPage0.hovered ? Design.colButtonDarkHoveredText : slCentral.currentIndex === 0 ? Design.colButtonDarkSelectedText : Design.colButtonDarkText
                    }

                    onHoveredChanged: checkMainMenuHovered();
                    onPressed: {globalManager.guiPage = page0_project; displayMenu = false;}
                }

                Button {
                    id: btCentralPage1
                    Layout.preferredWidth: 140
                    Layout.preferredHeight: 60
                    text: qsTr("PREPARER")
                    font.pointSize: 12
                    background: Rectangle {
                        color: Design.colButtonDarkBackground
                    }
                    contentItem: Text {
                        text: btCentralPage1.text
                        font: btCentralPage1.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        color: btCentralPage1.hovered ? Design.colButtonDarkHoveredText : slCentral.currentIndex === 1 ? Design.colButtonDarkSelectedText : Design.colButtonDarkText
                    }

                    onHoveredChanged: checkMainMenuHovered();
                    onPressed: {globalManager.guiPage = page1_prepare; displayMenu = false;}
                }

                Button {
                    id: btCentralPage2
                    Layout.preferredWidth: 140
                    Layout.preferredHeight: 60
                    text: qsTr("SUPERVISER")
                    font.pointSize: 12
                    background: Rectangle {
                        color: Design.colButtonDarkBackground
                    }
                    contentItem: Text {
                        text: btCentralPage2.text
                        font: btCentralPage2.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        color: btCentralPage2.hovered ? Design.colButtonDarkHoveredText : slCentral.currentIndex === 2 ? Design.colButtonDarkSelectedText : Design.colButtonDarkText
                    }

                    onHoveredChanged: checkMainMenuHovered();
                    onPressed: {globalManager.guiPage = page2_supervise; displayMenu = false;}
                }

                Button {
                    id: btCentralPage3
                    Layout.preferredWidth: 140
                    Layout.preferredHeight: 60
                    text: qsTr("SCANNER")
                    font.pointSize: 12
                    background: Rectangle {
                        color: Design.colButtonDarkBackground
                    }
                    contentItem: Text {
                        text: btCentralPage3.text
                        font: btCentralPage3.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        color: btCentralPage3.hovered ? Design.colButtonDarkHoveredText : slCentral.currentIndex === 3 ? Design.colButtonDarkSelectedText : Design.colButtonDarkText
                    }

                    onHoveredChanged: checkMainMenuHovered();
                    onPressed: {globalManager.guiPage = page3_scan; displayMenu = false;}
                }

                Button {
                    id: btCentralPage4
                    Layout.preferredWidth: 140
                    Layout.preferredHeight: 60
                    text: qsTr("AIDE")
                    font.pointSize: 12
                    background: Rectangle {
                        color: Design.colButtonDarkBackground
                    }
                    contentItem: Text {
                        text: btCentralPage4.text
                        font: btCentralPage4.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        color: btCentralPage4.hovered ? Design.colButtonDarkHoveredText : slCentral.currentIndex === 4 ? Design.colButtonDarkSelectedText : Design.colButtonDarkText
                    }

                    onHoveredChanged: checkMainMenuHovered();
                    onPressed: {globalManager.guiPage = page4_help; displayMenu = false;}
                }
            }
        }
    }

    onDisplayMenuChanged: {
        if( displayMenu ) {
            btMainMenu.icon.source = "qrc:///img/icons/btMainMenuClicked.png"
            btMainMenu.background.color = Design.colButtonDarkBackground

        } else {
            btMainMenu.icon.source = "qrc:///img/icons/btMainMenuNormal.png"
            btMainMenu.background.color = Design.colButtonDarkHoveredBackground
        }

        btCentralPage0.visible = displayMenu;
        btCentralPage1.visible = displayMenu;
        btCentralPage2.visible = displayMenu;
        btCentralPage3.visible = displayMenu;
        btCentralPage4.visible = displayMenu;
    }

    Connections {
        target : globalManager
        function onGuiPageChanged() {
            updateCentralPage();
        }
    }

    function updateCentralPage() {
        btCentralPage0.background.color = globalManager.guiPage === page0_project ? Design.colButtonDarkSelectedBackground: Design.colButtonDarkBackground
        btCentralPage1.background.color = globalManager.guiPage === page1_prepare ? Design.colButtonDarkSelectedBackground: Design.colButtonDarkBackground
        btCentralPage2.background.color = globalManager.guiPage === page2_supervise ? Design.colButtonDarkSelectedBackground: Design.colButtonDarkBackground
        btCentralPage3.background.color = globalManager.guiPage === page3_scan ? Design.colButtonDarkSelectedBackground: Design.colButtonDarkBackground
        btCentralPage4.background.color = globalManager.guiPage === page4_help ? Design.colButtonDarkSelectedBackground: Design.colButtonDarkBackground
        slCentral.currentIndex = globalManager.guiPage;
    }

    function checkMainMenuHovered() {
        var h = btMainMenu.hovered;
        h +=  btCentralPage0.hovered;
        h +=  btCentralPage1.hovered;
        h +=  btCentralPage2.hovered;
        h +=  btCentralPage3.hovered;
        h +=  btCentralPage4.hovered;

        displayMenu = h;
    }

    Component.onCompleted: {
        checkMainMenuHovered();
        updateCentralPage();
    }
}





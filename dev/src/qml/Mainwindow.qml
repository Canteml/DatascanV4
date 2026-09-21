import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0

Item {
    id: itemMain

    Layout.fillHeight: true
    Layout.fillWidth: true
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
    readonly property int page4_licence: 4;
    readonly property int page5_help: 5;

    property var displayMenu: false

    ColumnLayout {
        id: colMainLayout
        anchors.fill: parent
        spacing: 0

        LicenceWarning {}

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

            BannerVectorInfo {}

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
                CentralPage5 {}
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
                    text: qsTr("EQUIP.LICENCE")
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
                    onPressed: {globalManager.guiPage = page4_licence; displayMenu = false;}
                }

                Button {

                    id: btCentralPage5
                    Layout.preferredWidth: 140
                    Layout.preferredHeight: 60
                    text: qsTr("AIDE")
                    font.pointSize: 12
                    background: Rectangle {
                        color: Design.colButtonDarkBackground
                    }
                    contentItem: Text {
                        text: btCentralPage5.text
                        font: btCentralPage5.font
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        color: btCentralPage5.hovered ? Design.colButtonDarkHoveredText : slCentral.currentIndex === 5 ? Design.colButtonDarkSelectedText : Design.colButtonDarkText
                    }


                    onHoveredChanged: checkMainMenuHovered();
                    onPressed: {globalManager.guiPage = page5_help; displayMenu = false;}
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
        btCentralPage5.visible = false;
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
        btCentralPage4.background.color = globalManager.guiPage === page4_licence ? Design.colButtonDarkSelectedBackground: Design.colButtonDarkBackground
        btCentralPage5.background.color = globalManager.guiPage === page5_help ? Design.colButtonDarkSelectedBackground: Design.colButtonDarkBackground
        slCentral.currentIndex = globalManager.guiPage;
    }

    function checkMainMenuHovered() {
        var h = btMainMenu.hovered;
        h +=  btCentralPage0.hovered;
        h +=  btCentralPage1.hovered;
        h +=  btCentralPage2.hovered;
        h +=  btCentralPage3.hovered;
        h +=  btCentralPage4.hovered;
        h +=  btCentralPage5.hovered;

        displayMenu = h;
    }

    Component.onCompleted: {
        checkMainMenuHovered();
        slCentral.currentIndex = 1;
        Qt.callLater(function() {
            updateCentralPage();
        });
    }
}





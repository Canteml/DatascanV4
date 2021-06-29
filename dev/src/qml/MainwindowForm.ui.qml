import QtQuick 2.4
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import Design 1.0

Item {
    id: itemMain

    anchors.fill: parent
    property alias btBannerTitle: btBannerTitle
    property alias slCentral: slCentral

    property alias btMainMenu: btMainMenu
    //property alias colBannerInfo: colBannerInfo
    //    property alias lbBannerScan: lbBannerScan
    //    property alias lbBannerPiece: lbBannerPiece
    property alias rowVectorInfo: rowVectorInfo
    property alias rowBanner: rowBanner
    property alias colBtPage: colBtPage
    property alias btCentralPage0: btCentralPage0
    property alias btCentralPage1: btCentralPage1
    property alias btCentralPage2: btCentralPage2
    property alias btCentralPage3: btCentralPage3
    property alias btCentralPage4: btCentralPage4
    property alias itCentralView: itCentralView
    property alias colMainLayout: colMainLayout

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
            }

            Button {
                id: btBannerTitle
                text: qsTr("BAC D006\nGénératrice 8")
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
                    color: btBannerTitle.pressed ? Design.colButtonSelectedBackground : btBannerTitle.hovered ? Design.colButtonHoveredBackground : Design.colButtonBackground
                    //color: btBannerTitle.hovered ? "#9C9B9B" : "#FFFFFF"
                    border.width: 1
                    border.color: Design.colFrameBorder
                }
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
                }
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/


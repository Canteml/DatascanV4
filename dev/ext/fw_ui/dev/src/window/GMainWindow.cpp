/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : GMainWindow.cpp
# Author      : Thierry Lechevallier
# Date        : 30/07/2020
#
# ==========================================================================*/

#include "GMainWindow.h"
#include "ui_MainWindow.h"
#include <core/Core.h>

GMainWindow::GMainWindow(QWidget *parent)
    : QMainWindow(parent), XmlSettings("GMainWindow") , Loggable("GMainWindow")    
{

}

GMainWindow::~GMainWindow()
{
    delete m_pUi;
}

bool GMainWindow::initMainWindow()
{
    m_pUi = new Ui::MainWindow();
    m_pUi->setupUi(this);
    setWindowTitle(getWindowTitle());
    return initCustomMainWindow();
}

QString GMainWindow::getWindowTitle()
{
    return QString("%1 - %2").arg(Core::getInstance()->getApplicationName()).arg(Core::getInstance()->getVersion());
}

void GMainWindow::closeEvent(QCloseEvent *event)
{
    m_geometryAtClose = QString::fromLatin1(saveGeometry().toHex().data());
    m_stateAtClose = QString::fromLatin1(saveState().toHex().data());
    QMainWindow::closeEvent(event);
}


// -- XmlSettings : Inherited behaviour
bool GMainWindow::customLoadSettings(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE(QString, m_geometryAtClose);
    bRet &= XML_READ_UNIQUE(QString, m_stateAtClose);

    return bRet;
}

bool GMainWindow::customLoadDefaultSettings()
{
    return true;
}

bool GMainWindow::customApplySettings()
{
    QByteArray ba;
    ba = QByteArray::fromHex(m_geometryAtClose.toLatin1().data());
    restoreGeometry(ba);
    ba = QByteArray::fromHex(m_stateAtClose.toLatin1().data());
    restoreState(ba);
    return true;
}

bool GMainWindow::customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    XML_WRITE_UNIQUE(m_geometryAtClose);
    XML_WRITE_UNIQUE(m_stateAtClose);
    return true;
}

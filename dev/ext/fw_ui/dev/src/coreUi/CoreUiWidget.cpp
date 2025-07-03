/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : CoreUiWidget.cpp
# Author      : Thierry Lechevallier
# Date        : 03/06/2020
#
# ==========================================================================*/

// -- Qt includes
#include <QApplication>
#include <QMainWindow>
#include <QIcon>
#include <QFile>
#include <QThread>

// -- Internal includes
#include <coreUi/CoreUiWidget.h>
#include <window/GMainWindow.h>

// ----------------------------------------------------------------------------
// -- Static attributs of class CoreUiWidget

CoreUiWidget * CoreUiWidget::m_pCoreUiWidgetInstance = nullptr;

// ----------------------------------------------------------------------------
// -- Class Core

CoreUiWidget::CoreUiWidget(const QString &p_coreName) : Core(p_coreName), XmlSettings("CoreUiWidget")
{
    // init attributs :
    m_pCoreUiWidgetInstance = this;
    m_pCoreMainVindow = nullptr;
}

CoreUiWidget::~CoreUiWidget()
{
    // reset singleton pointer
    m_pCoreUiWidgetInstance = nullptr;
}

CoreUiWidget * CoreUiWidget::getInstanceCoreUiWidget()
{
    return m_pCoreUiWidgetInstance;
}

// -- Behaviour
int CoreUiWidget::execUiWidget(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    m_pApplication = new QApplication(argc, argv);

    return Core::exec(argc, argv);
}

QString CoreUiWidget::getDirAppResQml()
{
    return m_dirApp+"/res/qml";
}

// -- Core Behaviour

bool CoreUiWidget::initApp()
{
    // load settings
    loadSettings();

    return true;
}

bool CoreUiWidget::startApp()
{
    // -- load main interface
    bool bRet = true;

    m_pCoreMainVindow = makeMainWindow();

    if( nullptr == m_pCoreMainVindow ) {
        fLogError("makeMainWindow returns nullptr.");
        bRet = false;
    } else {
        m_pCoreMainVindow->loadSettings();
        m_pCoreMainVindow->initMainWindow();
        m_pCoreMainVindow->show();

        // set window icon
        QString icoPath = getDirAppResImg()+"/application.png";
        if( !QFile::exists(icoPath) ) {
            fLogError("Icon file not found : "+icoPath);
        } else {
            m_pCoreMainVindow->setWindowIcon(QIcon(icoPath));

        }

        m_pCoreMainVindow->show();
    }


    return bRet;
}

void CoreUiWidget::stopApp()
{
    if( nullptr != m_pCoreMainVindow ) {
        m_pCoreMainVindow->close();
    }
}

void CoreUiWidget::deleteApp()
{
    // -- delete Application
    if( nullptr != m_pCoreMainVindow ) {
        delete m_pCoreMainVindow;
        m_pCoreMainVindow = nullptr;
    }
}

void CoreUiWidget::help(QStringList & /*p_help*/) const
{

}

// -- XmlSettings : Inherited behaviour
bool CoreUiWidget::customLoadSettings(QDomElement * /*p_pParent*/)
{
    bool bRet = true;

    return bRet;
}

bool CoreUiWidget::customLoadDefaultSettings()
{
    return true;
}

bool CoreUiWidget::customApplySettings()
{
    return true;
}

bool CoreUiWidget::customSaveSettings(QDomDocument * /*p_pDoc*/, QDomElement * /*p_pParent*/)
{    
    return true;
}

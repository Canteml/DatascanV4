/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : CoreUiQml.cpp
# Author      : Thierry Lechevallier
# Date        : 03/06/2020
#
# ==========================================================================*/

// -- Qt includes
#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include <QFile>
#include <QQuickView>
#include <QQuickItem>
#include <QQmlProperty>
#include <QThread>
#include <QScreen>

#include <QTranslator>
#include <QInputDialog>


// -- Internal includes
#include <coreUi/CoreUiQml.h>

// ----------------------------------------------------------------------------
// -- Static attributs of class CoreUiQml

CoreUiQml * CoreUiQml::m_pCoreUiQmlInstance = nullptr;

// ----------------------------------------------------------------------------
// -- Class Core

CoreUiQml::CoreUiQml(const QString &p_coreName) : Core(p_coreName), XmlSettings("CoreUiQml")
{
    // init attributs :
    m_pCoreUiQmlInstance = this;
    m_pQmlApplicationEngine = nullptr;
    m_pGuiApplication = nullptr;
    m_pMainView = nullptr;
    m_pQmlTopLevel = nullptr;

    connect(this, &CoreUiQml::evReloadQml, this, &CoreUiQml::onReloadQml, Qt::QueuedConnection);
}

CoreUiQml::~CoreUiQml()
{
    // reset singleton pointer
    m_pCoreUiQmlInstance = nullptr;
}

CoreUiQml * CoreUiQml::getInstanceCoreUiQml()
{
    return m_pCoreUiQmlInstance;
}

// -- Behaviour
int CoreUiQml::execUiQml(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    m_pGuiApplication = new QApplication(argc, argv); //QGuiApplication(argc, argv);

    return Core::exec(argc, argv);
}


int CoreUiQml::execUiQml_withSelectLanguage(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    m_pGuiApplication = new QApplication(argc, argv); //QGuiApplication(argc, argv);


    //Language Selection
    QTranslator qTranslator;
    QStringList languages;
    languages << "French" << "English" ;
    QString qStrLang = QInputDialog::getItem(NULL,"Select Language",
                                         "Language",languages);

    //Loading translation file
    if("English" == qStrLang)
    {
        qTranslator.load(":/english.qm");
    }

    //Loading Translation on IHM
    if("French" != qStrLang)
    {
        m_pGuiApplication->installTranslator(&qTranslator);
    }

    return Core::exec(argc, argv);
}


int CoreUiQml::execUiQml_EnglishLanguage(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    m_pGuiApplication = new QApplication(argc, argv); //QGuiApplication(argc, argv);

    //Language Selection
    QTranslator qTranslator;
    //Loading translation file
    qTranslator.load(":/english.qm");
    //Loading Translation on IHM
    m_pGuiApplication->installTranslator(&qTranslator);


    return Core::exec(argc, argv);
}

int CoreUiQml::execUiQml_EspanolLanguage(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    m_pGuiApplication = new QApplication(argc, argv); //QGuiApplication(argc, argv);

    //Language Selection
    QTranslator qTranslator;
    //Loading translation file
    qTranslator.load(":/espanol.qm");
    //Loading Translation on IHM
    m_pGuiApplication->installTranslator(&qTranslator);


    return Core::exec(argc, argv);
}




QString CoreUiQml::getDirAppQml()
{
    if( isDevApp() ) {
        return QString("%1/../src/qml").arg(m_dirApp);
    } else {
        return m_dirApp+"/res/qml";
    }
}

QString CoreUiQml::getWindowTitle() const
{
    return QString("%1 %2").arg(getApplicationName()).arg(getVersion());
}

QQmlApplicationEngine *CoreUiQml::getApplicationEngine() const
{
    return m_pQmlApplicationEngine;
}

void CoreUiQml::updateWindowTitle()
{
    QQmlProperty(m_pQmlTopLevel, "title").write(getWindowTitle());
}

bool CoreUiQml::eventFilter(QObject * p_object, QEvent * p_event)
{
    if( QEvent::KeyRelease == p_event->type() && isDevApp() ) {
        QKeyEvent * kpe = (QKeyEvent *)p_event;
        if( Qt::Key_F5 == kpe->key() ) {
            emit evReloadQml();
        }
    }
    return QObject::eventFilter(p_object, p_event);
}

// -- Core Behaviour

bool CoreUiQml::initApp()
{
    m_pQmlApplicationEngine = new QQmlApplicationEngine();
    if( !useOpenGlRender() ) {
        // remove resizing artefact (blinking)
        QQuickWindow::setSceneGraphBackend(QSGRendererInterface::Software); // OpenGL introduces artefacts while resizing
    }

    m_mainQmlFile = QString("%1/main.qml").arg(getDirAppQml());
    m_mainQmlUrl.setUrl(QUrl::fromLocalFile(m_mainQmlFile.toLatin1()).toString());

    // -- add path
    initQmlPath();

    // -- Add Types
    initQmlType();


    return true;
}

bool CoreUiQml::startApp()
{
    // -- load main interface
    bool bRet = true;
    if( QFile::exists(m_mainQmlFile) ) {

        connect(m_pQmlApplicationEngine, &QQmlApplicationEngine::objectCreated,this, &CoreUiQml::onQmlCreated, Qt::QueuedConnection);

        // -- set context
        initQmlContext();

        m_pQmlApplicationEngine->load(m_mainQmlUrl);

        // set window icon
        QString icoPath = getDirAppResImg()+"/application.png";
        if( !QFile::exists(icoPath) ) {
            fLogError("Icon file not found : "+icoPath);
        } else {
            m_pGuiApplication->setWindowIcon(QIcon(icoPath));

        }
    } else {
        bRet = false;
        fLogError("File not found : " + m_mainQmlFile);
    }

    return bRet;
}

void CoreUiQml::stopApp()
{
    // read some window properties
    //m_settWinVisibility = m_pMainView->visibility();
    if( nullptr != m_pMainView ) {
        m_settWinGeometry = m_pMainView->geometry();
        m_pMainView = nullptr;
    }

//    QByteArray ba;
//    QDataStream ds(&ba, QIODevice::WriteOnly);
//    ds<<m_pMainView->geometry();
//    m_settWinGeometry = ba.toHex().data();

    // -- delete Qml objects
    if( nullptr != m_pQmlApplicationEngine ) {
        delete m_pQmlApplicationEngine;
        m_pQmlApplicationEngine = nullptr;
    }

}

void CoreUiQml::deleteApp()
{
    // -- delete Application
    if( nullptr != m_pGuiApplication ) {
        delete m_pGuiApplication;
        m_pGuiApplication = nullptr;
    }

    m_pMainView = nullptr;
}

void CoreUiQml::initCppFromQml()
{
    // implemented empty
}

void CoreUiQml::help(QStringList & /*p_help*/) const
{

}

bool CoreUiQml::useOpenGlRender() const
{
    return false;
}

void CoreUiQml::onQmlCreated(QObject *object, const QUrl &url)
{
    // -- Called just after main window has been loaded
    if (!object) {
        fLogError("Fail to create qml : " + url.toString());
        QCoreApplication::exit(-1);
    } else {
        m_pQmlTopLevel = object;
        m_pMainView = qobject_cast<QQuickWindow *>(m_pQmlTopLevel);

        initCppFromQml();

        // install filter to catch keyboard event :
        m_pMainView->installEventFilter(this);

        if( nullptr == m_pMainView ) {
            fLogError("m_pMainView is NULL, main.qml must contain a ApplicationWindow.");
            qApp->exit(-1);
        } else {
            // connections with qml
            connect(m_pMainView, &QWindow::visibilityChanged, this, &CoreUiQml::onWinVisibilityChanged);

            // set proporties of main window
            updateWindowTitle();

            // load settings
            loadSettings();
        }
    }
}

void CoreUiQml::onWinVisibilityChanged(QWindow::Visibility p_visibility)
{
    // Do not take care of Hidden
    if( QWindow::Hidden != p_visibility ) {
        m_settWinVisibility = p_visibility;
    }
}

void CoreUiQml::onReloadQml()
{
    for( QObject * obj : m_pQmlApplicationEngine->rootObjects() ) {
        m_pQmlApplicationEngine->rootObjects().removeOne(obj);
        m_pQmlApplicationEngine->setObjectOwnership(obj, QQmlApplicationEngine::CppOwnership);
        obj->deleteLater();

    }
//    m_pQmlApplicationEngine->loadData(QByteArray());
//    m_pQmlApplicationEngine->clearComponentCache();
//    m_pQmlApplicationEngine->load(m_mainQml);

    m_pQmlApplicationEngine->clearComponentCache();
    m_pQmlApplicationEngine->load(m_mainQmlUrl);
}

// -- XmlSettings : Inherited behaviour
bool CoreUiQml::customLoadSettings(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE(quint32, m_settWinVisibility);
    QString sWinGeo;
    bRet &= XML_READ_UNIQUE(QString, sWinGeo);
    QByteArray ba = QByteArray::fromHex(sWinGeo.toLatin1().data());
    QDataStream ds(&ba, QIODevice::ReadOnly);
    ds>>m_settWinGeometry;

    return bRet;
}

bool CoreUiQml::customLoadDefaultSettings()
{
    m_settWinVisibility = QWindow::AutomaticVisibility;
    m_settWinGeometry = QRect(100,100,740,580);

    return true;
}

bool CoreUiQml::customApplySettings()
{
    m_pMainView->setVisibility((QWindow::Visibility)m_settWinVisibility);

    QList<QScreen *> lScreens = QGuiApplication::screens();
    bool bOutside = true;
    for(auto s : qAsConst(lScreens)) {
        if( s->availableGeometry().contains(m_settWinGeometry) ) {
            bOutside = false;
        }
    }
    if( bOutside ) {
        // window out of screens, can happen if the window was on an other screen removed (dual screen => single screen)
        m_settWinGeometry = QRect(100,100,740,580);
        fLogWarning("load default window position");
    }
    m_pMainView->setGeometry(m_settWinGeometry);

    return true;
}

bool CoreUiQml::customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    XML_WRITE_UNIQUE(m_settWinVisibility);
    QString sWinGeo;
    QByteArray ba;
    QDataStream ds(&ba, QIODevice::WriteOnly);
    ds<<m_settWinGeometry;
    sWinGeo = ba.toHex().data();
    XML_WRITE_UNIQUE(sWinGeo);
    return true;
}

/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Core.cpp
# Author      : Thierry Lechevallier
# Date        : 13/05/2020
#
# ==========================================================================*/

// -- Qt includes
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QTextStream>

// -- Std includes
#ifdef _WIN32
#include <QTextStream>
#include <iostream>
#include <csignal>
#endif
#ifdef __linux__
#include <initializer_list>
#include <signal.h>
#include <unistd.h>
#endif

// -- Internal includes
#include <core/Core.h>
#include <xml/XmlSettings.h>

#include <QMetaType>
#include "VectorFuncAScan.h"
#include "GenComLink.h"
#include "ComDevice.h"
#include "ComDeviceManager.h"

void registerMetaTypes() {
    qRegisterMetaType<VectorFuncAScan*>();
    qRegisterMetaType<GenComLink*>();
    qRegisterMetaType<ComDevice*>();
    qRegisterMetaType<ComDeviceManager*>();
}
// ----------------------------------------------------------------------------
// -- Static attributs of class Core

Core * Core::m_pCoreInstance = nullptr;
QString Core::m_dirApp;
QString Core::m_dirData;

// ----------------------------------------------------------------------------
// -- System Signal
#ifdef _WIN32
void signalHandler(int signum )
{   // https://www.tutorialspoint.com/cplusplus/cpp_signal_handling.htm
    Logger::logFatal(QString("Interrupt signal : %1").arg(signum));
    // Exit application
    qApp->quit();
}
#endif

#ifdef _linux__
// https://gist.github.com/azadkuh/a2ac6869661ebd3f8588
void catchUnixSignals(std::initializer_list<int> quitSignals) {
    Logger::logFatal("Interrupt signal.");
    auto handler = [](int sig) -> void {
        // blocking and not aysnc-signal-safe func are valid
        printf("\nquit the application by signal(%d).\n", sig);
        QCoreApplication::quit();
    };

    sigset_t blocking_mask;
    sigemptyset(&blocking_mask);
    for (auto sig : quitSignals)
        sigaddset(&blocking_mask, sig);

    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_mask    = blocking_mask;
    sa.sa_flags   = 0;

    for (auto sig : quitSignals)
        sigaction(sig, &sa, nullptr);
}
#endif

// ----------------------------------------------------------------------------
// -- Class Core

Core::Core(const QString &p_coreName) : Loggable(p_coreName)
{
    // init attributs :
    m_pCoreInstance = this;
    m_appReturn = 0;    // by default, application ends with no error
    m_pLogger = nullptr;
    m_dirApp = "";
    m_dirData = "";

    // catch signal from system :
    #ifdef _WIN32
    signal(SIGINT, signalHandler);
    signal(SIGILL, signalHandler);
    signal(SIGABRT_COMPAT, signalHandler);
    signal(SIGFPE, signalHandler);
    signal(SIGSEGV, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGBREAK, signalHandler);
    signal(SIGABRT, signalHandler);
    signal(SIGABRT2, signalHandler);
    #endif
    #ifdef __linux__
    catchUnixSignals({SIGQUIT, SIGINT, SIGTERM, SIGHUP});
    #endif
}

Core::~Core()
{
    // delete m_lArgOption
    for(auto i : qAsConst(m_lArgOption) ) {
        delete i;
    }
    m_lArgOption.clear();
    m_lArgArgument.clear();

    // reset singleton pointer
    m_pCoreInstance = nullptr;
}

Core * Core::getInstance()
{
    return m_pCoreInstance;
}

// -- Behaviour
int Core::exec(int argc, char* argv[])
{
    registerMetaTypes(); // <-- registra qui i tipi

    bool bContinue = true;
    // -- init app names
    qApp->setOrganizationName(getCompagnyName());
    qApp->setOrganizationDomain(getCompagnyDomain());
    qApp->setApplicationName(getApplicationName());
    qApp->setApplicationVersion(getVersion());

    // -- init app names
    qApp->setOrganizationName(getCompagnyName());
    qApp->setOrganizationDomain(getCompagnyDomain());
    qApp->setApplicationName(getApplicationName());
    qApp->setApplicationVersion(getVersion());

    // -- state chart

    processArguments(argc, argv);

    bContinue = coreInitApp();

    if( bContinue ) {
        bContinue = coreStartApp();

        if( bContinue ) {
            // Enters the main event loop and waits until exit() is called
            m_appReturn = qApp->exec();

            // exit() has been called
            coreStopApp();
        }
    }
    coreDeleteApp();

    return m_appReturn;
}

// -- Data
QString Core::getVersion() const
{
    return "0.0.0";
}

// Constants
QString Core::getCompagnyName() const
{
    return "Roboplanet";
}

QString Core::getCompagnyDomain() const
{
    return "www.roboplanet.fr";
}


// -- Software common directories
QString Core::getDirApp()
{
    return m_dirApp;
}

QString Core::getDirAppBin()
{
    return m_dirApp+"/bin";
}

QString Core::getDirAppLib()
{
    return m_dirApp+"/lib";
}

QString Core::getDirAppRes()
{
    return m_dirApp+"/res";
}

QString Core::getDirAppResImg()
{
    return m_dirApp+"/res/img";
}

QString Core::getDirData()
{
    return m_dirData;
}

QString Core::getDirDataConf()
{
    return m_dirData+"/conf";
}

QString Core::getDirDataLog()
{
    return m_dirData+"/log";
}

QString Core::getDirDataData()
{
    return m_dirData+"/data";
}

QString Core::getDirDataTmp()
{
    return m_dirData+"/tmp";
}

QString Core::getDirDataLicence()
{
    return m_dirData+"/licence";
}

bool Core::makeDirIfAbsent(const QString &p_dir)
{
    bool bRet = true;
    if( !QDir(p_dir).exists() ) {
        bRet = QDir().mkpath(p_dir);
        if( !bRet ) {
            Logger::logError("Core::makeDirIfAbsent - Fail to make path " + p_dir);
        }
    }
    return bRet;
}

bool Core::isArgumentSet(const QString &pArg)
{
    bool bFound = false;
    for(auto i : qAsConst(m_lArgArgument)) {
        if( i == pArg ) {
            bFound = true;
        }
    }
    return bFound;
}

bool Core::isArgumentSet(const QStringList & pArg)
{
    for(auto i : qAsConst(pArg)) {
        if( isArgumentSet(i )) {
            return true;
        }
    }
    return false;
}

QStringList Core::getOptionParameters(const QStringList & pOptionName)
{
    QStringList slRet;

    for(auto i : qAsConst(m_lArgOption)) {
        // For each item of the list of option, find if the option name match :
        if( pOptionName.contains(i->m_optName, Qt::CaseInsensitive) && slRet.isEmpty() ) {
            slRet = i->m_optParams;
            if( slRet.size() == 0 ) {
                slRet<<""; // add an empty item to signal that the option has been found but no parameters set for it.
            }
        }
    }

    return slRet;
}

bool Core::isDevApp()
{
    QDir appDir = QCoreApplication::applicationDirPath(); // Application  directory
    appDir.makeAbsolute();
    return appDir.path().right(4) != "/bin";
}


// -- Behaviour
// State chart
bool Core::coreInitApp()
{

    // MODIFICA  DA QUI

    // MODIFICA  FINO A QUI

    bool bContinue = true;
    bool initOk = false;

    // init paths :
    QDir appPath = QCoreApplication::applicationDirPath() + "/.."; // Application is in /bin directory, get parent directory
    bContinue &= appPath.makeAbsolute();


    if( !bContinue ) {
        qDebug()<<"Core::coreInitApp : Fail to make absolute path from " + appPath.path();
    } else {
        m_dirApp = appPath.path();
        #ifdef _WIN32
        m_dirData = "C:/ProgramData/" + getCompagnyName() + "/" + getApplicationName();
        #endif
        #ifdef __linux__
        m_dirData = QStandardPaths::displayName(QStandardPaths::AppDataLocation) + getCompagnyName() + "/" + getApplicationName();
        #endif

        // Make data directories if absent
        bContinue &= makeDirIfAbsent(getDirData());
        bContinue &= makeDirIfAbsent(getDirDataConf());
        bContinue &= makeDirIfAbsent(getDirDataLog());
        bContinue &= makeDirIfAbsent(getDirDataData());
        bContinue &= makeDirIfAbsent(getDirDataTmp());
        bContinue &= makeDirIfAbsent(getDirDataLicence());
    }

    if( bContinue ) {
        if( !getOptionParameters(QStringList()<<"-h"<<"--help").isEmpty() ) {
            printHelp();
            bContinue = false; // Stop program without error if help is printed
            initOk = true;
        }
    }

    if( bContinue ) {
        // Load settings

        XmlSettings::readSettingsFromFile();

        // Instanciate Logger
        m_pLogger = Logger::makeInstance();

        bContinue = m_pLogger->loadSettings();

    }

    if( bContinue ) {
        // Init application
        bContinue = initApp();

    }

    if( !initOk && !bContinue) {
        m_appReturn = 1;
    }
    return bContinue;
}

bool Core::coreStartApp()
{
    bool bContinue = true;

    m_pLogger->startLogger();

    // Start application
    bContinue = startApp();

    if( !bContinue ) {
        m_appReturn = 2;
    }

    return bContinue;
}

void Core::coreStopApp()
{
    // Stop application
    stopApp();

    // Save settings
    XmlSettings::writeSettingsToFile();

    // get fatal errors
    const QStringList pslFatals = Logger::getFatalErrors();
    if( 0 != pslFatals.size() ) {
        m_appReturn = 3; // Fatal detected during execution
    }
    Logger::logInfo(QString("== Application returns %1 ==").arg(m_appReturn));

    // Stop Logger
    m_pLogger->stopLogger();
}

void Core::coreDeleteApp()
{
    // Delete application
    deleteApp();

    // delete logger
    if( nullptr != m_pLogger ) {
        delete m_pLogger;
        m_pLogger = nullptr;
    }

    XmlSettings::deleteXmlDoc();

    const QStringList pslFatals = Logger::getFatalErrors();
    // process fatal errors
    if( pslFatals.size() ) {
        #ifdef _WIN32
        QTextStream(stdout)<<"\r\n\e[48;5;11m\033[38;5;9m == List of FATAL ERRORS =="<<Qt::endl;
        #endif
        #ifdef __linux__
        qDebug()<<"\e[48;5;11m\033[38;5;9m"<<"\r\n == List of FATAL ERRORS ==";
        #endif
        for( auto fatal : pslFatals) {
            QTextStream(stdout)<<fatal.toLatin1().data()<<Qt::endl;
        }
    }

    // Rest terminal color to white and black
    #ifdef _WIN32
        QTextStream(stdout)<<"\033\[49m\033[0m"<<Qt::endl;
    #endif
    #ifdef __linux__
        qDebug()<<"e\[49\033[0m";
    #endif

}

void Core::printHelp()
{
    QStringList slHelp;
    QString title = QString("Help for %1 (version %2)").arg(getApplicationName()).arg(getVersion());
    slHelp<<QString("-").repeated(title.size());
    slHelp<<title;
    slHelp<<QString("-").repeated(title.size());
    slHelp<<"";
    slHelp<<"# Common commands : ";
    slHelp<<"";
    slHelp<<"    -h or --help  : display this help";
    slHelp<<"    -d or --data  : set application data directory";
    slHelp<<"";
    QStringList specialCommands;
    help(specialCommands);

    if( specialCommands.size() ) {
        slHelp<<"# Special commands : ";
        slHelp<<specialCommands;
    }

    #ifdef _WIN32
        QTextStream(stdout)<<"\033\[49m\033[0m"<<Qt::endl;
    #endif
    #ifdef __linux__
        qDebug()<<"e\[49\033[0m";
    #endif

    foreach(QString s, slHelp) {
        qInfo()<<s.toLatin1().data();
    }

}

void Core::processArguments(int &argc, char *argv[])
{
    for(int iArg = 1; iArg<argc; iArg++) {
        QString sArg(argv[iArg]);
        if( "-" == sArg.left(1) ) {
            // We are parsing an option
            ArgOption * pOpt = new ArgOption();
            pOpt->m_optName = sArg;

            // Continue parsing to find paramaters (if exist)
            iArg++;
            bool bOptHasParam = true;

            for(; iArg<argc && bOptHasParam; iArg++) {
                QString sParam(argv[iArg]);
                if( "-" == sParam.left(1) ) {
                    // It's a begin of the next option => no parameters
                    bOptHasParam = false;
                    iArg -=2;
                } else {
                    pOpt->m_optParams.append(sParam);
                }
            }
            m_lArgOption.append(pOpt);
        }
        else {
            // It's an argument
            m_lArgArgument.append(sArg);
        }
    }
}


// Custom behaviour for inherited classes (implemented empty)
bool Core::initApp()
{
    // implementad empty
    return true;
}

bool Core::startApp()
{
    // implementad empty
    return true;
}

void Core::stopApp()
{
    // implementad empty
}

void Core::deleteApp()
{
    // implementad empty
}

void Core::help(QStringList &) const
{
    // implemented empty
}



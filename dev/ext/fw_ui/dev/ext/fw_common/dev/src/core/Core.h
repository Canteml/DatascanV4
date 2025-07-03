/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Core.h
# Author      : Thierry Lechevallier
# Date        : 13/05/2020
#
# ==========================================================================*/

#pragma once

// -- Qt inccludes
#include <QObject>
#include <QCoreApplication>

// -- internal includes
#include <log/Logger.h>
#include <log/Loggable.h>


// ----------------------------------------------------------------------------
// -- Class argOption
/**
 * @brief The argOption class
 *
 * This class associates an option name with o list of parameters
 */
class ArgOption
{
public :
    ArgOption() {}              ///< Empty constructor
    QString m_optName;          ///< Option Name
    QStringList m_optParams;    ///< List of parameters of the option
};

// ----------------------------------------------------------------------------
// -- Class Core
/**
 * @brief The Core class
 *
 * Main features :
 * - Start/stop the application.
 * - Manage Logger
 * - Give constants like the directories to use, the application name, version ...
 * - Treat application arguments (argc/argv)
 * - Display HELP
 *
 * How arguments and options works :
 * The arguments passed to an application are ordered, and the syntax is the following :
 * application a1 a2 ... -o1 p1 p2 --option2 p1 p2 p3
 * - the elements "a1 a2 ..." are named "arguments". They are present or absent.
 *   the most common argument know is "help" : display application help then quit.
 * - the elements "-o1" and "--option2" are called "options". Use one "-" for short name (one letter), "--" for long name.
 *   an option can have parameters ("p1 p2 ..."), separated by one space.
 */
class Core : public Loggable
{
public :
    Core(const QString& p_coreName);                     ///< public constructor to be inherited
    virtual ~Core();
    static Core * getInstance();    ///< get core instance

    // -- Behaviour
    int exec(int argc, char* argv[]);

    // Constants
    virtual QString getCompagnyName() const;
    virtual QString getCompagnyDomain() const;

    // Specified by inherited classes
    virtual QString getVersion() const;
    virtual QString getApplicationName() const = 0;

    // -- Software common directories
    static QString getDirApp();     ///< Application directory (C:\Program Files\Company Name\App Name)
    static QString getDirAppBin();  ///< Binaries & dependencies (win)
    static QString getDirAppLib();  ///< Libraries
    static QString getDirAppRes();  ///< Resources
    static QString getDirAppResImg();  ///< Resources images
    static QString getDirData();    ///< Application data (workspace) (C:\ProgramData\Company Name\App Name)
    static QString getDirDataConf();    ///< settings, permissions...
    static QString getDirDataLog();     ///< log files
    static QString getDirDataData();    ///< data persisted after exit
    static QString getDirDataTmp();     ///< data not persisted after exit

    static bool makeDirIfAbsent(const QString & p_dir);

    bool isArgumentSet(const QString & pArg);
    bool isArgumentSet(const QStringList & pArg);   ///< set multiple syntax of the argument to found (example: "--h", "-help ...)
    QStringList getOptionParameters(const QStringList & pOptionName);   ///< return a list with the options for a given parameter. Return an empty list if option not found. Return a list with on empty item if option found but no parameters are set.

protected :
    // -- Behaviour
    // State chart
    bool coreInitApp();     ///< Make common managers (Logger) and prepare settings
    bool coreStartApp();    ///< Start managers
    void coreStopApp();     ///< Stop managers
    void coreDeleteApp();   ///< Delete managers, save settings

    void printHelp();

    // Arguments
    void processArguments(int &argc, char * argv[]);

    // Custom behaviour for inherited classes (implemented empty)
    virtual bool initApp();     ///< Make managers
    virtual bool startApp();    ///< Start managers
    virtual void stopApp();     ///< Stop managers
    virtual void deleteApp();   ///< Delete managers

    virtual void help(QStringList & p_help) const;    ///< inherited class can add lines to the help

    // -- Attributs
    // Behaviour
    static Core * m_pCoreInstance;
    int m_appReturn;    ///< 0 = no error, else : error
    Logger * m_pLogger;
    QList<ArgOption*> m_lArgOption; ///< List of options
    QStringList m_lArgArgument;     ///< list of arguments

    // Directories
    static QString m_dirApp;
    static QString m_dirData;
};

/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Logger.h
# Author      : Thierry Lechevallier
# Date        : 14/05/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlSettings.h>

// -- Qt includes
#include <QString>
#include <QObject>
#include <QMutex>
class QThread;
class QFile;

/**
 * @brief The logMessage class
 *
 * Class to define a message that will be logged
 */
class logMessage
{
public :
    enum LOG_LEVEL {
        LOG_LEVEL_FATAL,
        LOG_LEVEL_ERROR,
        LOG_LEVEL_WARNING,
        LOG_LEVEL_INFO,
        LOG_LEVEL_DEBUG1,
        LOG_LEVEL_DEBUG2,
        LOG_LEVEL_MAX = 1000
    };

    logMessage(quint32 p_level, const QString & p_message, const QString & p_header = "");   ///< constructor, will set the header if enmpty ("29.05.2020 15:23:47.846 -")

    quint32 m_level;    ///< value in LOG_LEVEL
    QString m_header;
    QString m_message;
};

/**
 * @brief The Logger class
 *
 * This class has settings :
 * - log in file (true / false) : default = true
 * - max log lines in a file    : default = 10000
 * - verbosity (in LOG_LEVEL)   : default = MAX
 *
 */
class Logger : public QObject, public XmlSettings
{
    Q_OBJECT
public:
    static Logger * makeInstance();
    static Logger * getInstance();
    ~Logger();

    // -- state chart
    bool initLogger();  ///< initialize the logger
    bool startLogger(); ///< start the logger
    void stopLogger();  ///< stop the logger

    // -- Settings
    void setLogInFile(bool p_settingLogFile);
    bool getLogInFile() const;
    void setMaxFileLines(quint32 p_settingFileLines);
    quint32 getMaxFileLines() const;
    static void setVerbosity(quint32 p_maxLogLevel);
    static quint32 getVerbosity();

    // -- Static log methods    
    static void logFatal(const QString & p_message);
    static void logError(const QString & p_message);
    static void logWarning(const QString & p_message);
    static void logInfo(const QString & p_message);
    static void logDebug1(const QString & p_message);
    static void logDebug2(const QString & p_message);

    // -- Behaviour
    static const QStringList & getFatalErrors();
    static void addFatalError(const QString & p_message);
    static void addLog(logMessage* p_log);
    static bool isRunning();

protected :
    // -- XmlSettings
    bool customLoadSettings(QDomElement * p_pParent) override;
    bool customLoadDefaultSettings() override;
    bool customApplySettings() override;
    bool customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

private slots :
    void onProcessOneLog(); ///< executed in the thread m_thLogger

signals :
    void evProcessOneLog();
    void evNewLog(quint32 level, const QString &header, const QString & message);

private :
    Logger();

    void openLogFile();
    void rollLogFile();
    void closeLogFile(QString p_renameFile = "");

    void startProcessingIfNeeded();

    // ---- Members
    static Logger * m_pMyInstance;

    // -- settings
    bool m_logFile;
    quint32 m_maxFileLines;
    static quint32 m_verbosity;

    // -- Behaviour
    static volatile bool m_isProcessingLog;
    QString m_logHeader;    ///< for files
    static QStringList m_lFatalErrors; ///< List of fatals errors treated by the core when the program ends
    static QList<logMessage*> m_lLogs;   ///< List with logs to treat
    static QRecursiveMutex m_mLogs;      ///< protect m_lLogs
    static bool m_isFinished;

    static QThread * m_thLogger;   ///< thread to treal logs

    QString m_logFileName;
    QString m_logFileNameOld;   ///< new name when file is full
    static quint32 m_countLogLost;   ///< log lost if the buffer is full (The list m_lLogs has more than MAX_LIST_LOG_SIZE items and we're trying to add one more log)

    // file
    QFile * m_pLogFile;         ///< current file where logs are written
    static QRecursiveMutex m_mLogFile;   ///< protect file access to m_pLogFile
    quint32 m_fileLineCount;    ///< number lines in the cuurent log file

    // -- constants
    static int MAX_LIST_LOG_SIZE;

};


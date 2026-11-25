/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Logger.cpp
# Author      : Thierry Lechevallier
# Date        : 14/05/2020
#
# ==========================================================================*/

// -- Qt includes
#include <QThread>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <QFileInfo>
#include <QRecursiveMutex>

#include <iostream>

// -- Internal includes
#include <log/Logger.h>
#include <core/Core.h>

// ----------------------------------------------------------------------------
// -- Class logMessage

logMessage::logMessage(quint32 p_level, const QString & p_message, const QString & p_header)
{
    m_level = p_level;
    m_message = p_message;
    if( p_header.isEmpty() ) {
        m_header = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz");
    } else {
        m_header = p_header;
    }
}

QThread * Logger::m_thLogger = nullptr;

// ----------------------------------------------------------------------------
// -- Class Logger

// -- constants
#define VERBOSITY_ID_SIZE 6
char VERBOSITY_ID[VERBOSITY_ID_SIZE] = {'F', 'E', 'W', 'I', '1', '2'};

// -- Windows definitions
#ifdef _WIN32
#include <windows.h>
// Source : https://ss64.com/nt/color.html
enum console_colors { BLACK, DARKBLUE, DARKGREEN, DARKTEAL, DARKRED, DARKPINK, DARKYELLOW, GRAY, DARKGRAY, BLUE, GREEN, TEAL, RED, PINK, YELLOW, WHITE};

void setConsoleColor(const WORD & p_color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), p_color);
}

void setConsoleForegroundAnBackgroundColor(int p_foreground, int p_background)
{
    int color = p_background*16 + p_foreground;
    setConsoleColor(color);
}
#endif

void RedirectQtMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // compute localisation
    QString loc = context.function;
    if(loc.isEmpty()){
        if( !QString(context.file).isEmpty()  ) {
            if (-1 != context.line ) {
                loc = QString("%1").arg(QFileInfo(context.file).fileName());
            } else {
                loc = QString("%1::%2").arg(QFileInfo(context.file).fileName()).arg(context.line);
            }
        }
    }
    if( loc.isEmpty() ) {
        loc = "Qt";
    }

    switch (type) {
    case QtDebugMsg:
        Logger::logDebug1(QString("%1 : %2").arg(loc).arg(msg));
        break;
    case QtInfoMsg:
        Logger::logInfo(QString("%1 : %2").arg(loc).arg(msg));
        break;
    case QtWarningMsg:
        Logger::logWarning(QString("%1 : %2").arg(loc).arg(msg));
        break;
    case QtCriticalMsg:
        Logger::logError(QString("%1 : %2").arg(loc).arg(msg));
        break;
    case QtFatalMsg:
        Logger::logFatal(QString("%1 : %2").arg(loc).arg(msg));
        break;
    }
}

// -- Static members
Logger * Logger::m_pMyInstance = nullptr;

QStringList Logger::m_lFatalErrors;
QList<logMessage*> Logger::m_lLogs;
QRecursiveMutex Logger::m_mLogs;
QRecursiveMutex Logger::m_mLogFile;
volatile bool Logger::m_isProcessingLog = false;
quint32 Logger::m_verbosity = logMessage::LOG_LEVEL_MAX;
bool Logger::m_isFinished = false;

quint32 Logger::m_countLogLost = 0;

int Logger::MAX_LIST_LOG_SIZE = 10000;

// -- Logger definitions
Logger::Logger() : XmlSettings("Logger")
{
    qInstallMessageHandler(RedirectQtMessage); // Install the handler

    // init members :
    m_logFile = true;
    m_maxFileLines = 10000;
    m_thLogger = new QThread();
    m_pLogFile = nullptr;
    m_logFileName = QString("%1/log.txt").arg(Core::getDirDataLog());
    m_logFileNameOld = QString("%1/_log.txt").arg(Core::getDirDataLog());
    m_fileLineCount = 0;

    // Make header
    Core * pCore = Core::getInstance();
    QString appName = QString("=== Application %1 (V %2) started at %3 ===").arg(pCore->getApplicationName()).arg(pCore->getVersion()).arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"));
    QString line = QString().fill('-', appName.size());
    m_logHeader = QString("%1\r\n%2\r\n%3\r\n").arg(line).arg(appName).arg(line);

    // add header in the logs
    logInfo(line);
    logInfo(appName);
    logInfo(line);

    // Prepare thread
    moveToThread(m_thLogger);

    // Connection executed in thread m_thLogger
    connect(this, &Logger::evProcessOneLog, this, &Logger::onProcessOneLog, (Qt::ConnectionType)(Qt::UniqueConnection | Qt::QueuedConnection)); // allow only one request to treat logs
}

Logger * Logger::makeInstance()
{
    if( nullptr != m_pMyInstance ) {
        logError("Logger::makeInstance - Logger is already running");
    } else {
        m_pMyInstance = new Logger();
    }
    return m_pMyInstance;
}

Logger * Logger::getInstance()
{
    return m_pMyInstance;
}

Logger::~Logger()
{
    // check that the logger can properly be deleted :
    if( isRunning() || m_lLogs.size()) {
        stopLogger();
        if(m_lLogs.size()) {
               qDebug()<<QString("Logger::~Logger - Still %1 logs to treat").arg(m_lLogs.size());
        }
    }

    closeLogFile();

    // delete objects
    disconnect(this, &Logger::evProcessOneLog, this, &Logger::onProcessOneLog);
    delete m_thLogger;
    m_thLogger = nullptr;

    // restore colors in the console
    #ifdef _WIN32
        setConsoleForegroundAnBackgroundColor(GRAY, BLACK);
    #else
        qDebug()<<"\033[0m[";
    #endif

    m_isFinished = true;
    m_pMyInstance = nullptr;
}

// -- state chart
bool Logger::initLogger()
{
    // load settings
    return loadSettings();
}

bool Logger::startLogger()
{
    bool bRet = true;
    if( isRunning() ) {
        logError("Logger::startLogger - Logger aldready running");
        bRet = false;
    } else {
        // Start thread with low priority
        m_thLogger->start(QThread::LowPriority);

        // If some logs has already been buffered, then treat them
        startProcessingIfNeeded();
    }
    return bRet;
}

void Logger::stopLogger()
{
    // Stop thread
    if( isRunning() ) {
        m_thLogger->quit();
        m_thLogger->wait(1000); // wait thread end
    }
    // Tread remaining logs (just in case)
    while(m_lLogs.size()) {
        onProcessOneLog();
    }
}

// -- Settings
void Logger::setLogInFile(bool p_settingLogFile)
{
    m_mLogFile.lock();
    m_logFile = p_settingLogFile;
    if( m_logFile ) {
        if( m_pLogFile == nullptr ) {
            // create the file
            m_pLogFile = new QFile(m_logFileName);
            if( !m_pLogFile->open(QIODevice::ReadWrite | QIODevice::Append) ) {
                delete m_pLogFile;
                m_pLogFile = nullptr;
                logError(QString("Logger::setLogInFile - Fail to open file to log : %1").arg(m_logFileName));
            } else {
                // count the number of lines
                QTextStream in(m_pLogFile);
                m_fileLineCount = 0;
                while( !in.atEnd()) {
                    m_fileLineCount++;
                }
            }
        }
    }
    m_mLogFile.unlock();

}

bool Logger::getLogInFile() const
{
    return m_logFile;
}

void Logger::setMaxFileLines(quint32 p_settingFileLines)
{
    m_maxFileLines = p_settingFileLines;
}

quint32 Logger::getMaxFileLines() const
{
    return m_maxFileLines;
}

void Logger::setVerbosity(quint32 p_maxLogLevel)
{
    m_verbosity = p_maxLogLevel;
}

quint32 Logger::getVerbosity()
{
    return m_verbosity;
}

// -- log methods
void Logger::logFatal(const QString & p_message)
{
    Logger::getInstance()->addLog(new logMessage(logMessage::LOG_LEVEL_FATAL, p_message));
}

void Logger::logError(const QString & p_message)
{
    Logger::getInstance()->addLog(new logMessage(logMessage::LOG_LEVEL_ERROR, p_message));
}

void Logger::logWarning(const QString & p_message)
{
    Logger::getInstance()->addLog(new logMessage(logMessage::LOG_LEVEL_WARNING, p_message));
}

void Logger::logInfo(const QString & p_message)
{
    Logger::getInstance()->addLog(new logMessage(logMessage::LOG_LEVEL_INFO, p_message));
}

void Logger::logDebug1(const QString & p_message)
{
    Logger::getInstance()->addLog(new logMessage(logMessage::LOG_LEVEL_DEBUG1, p_message));
}

void Logger::logDebug2(const QString & p_message)
{
    Logger::getInstance()->addLog(new logMessage(logMessage::LOG_LEVEL_DEBUG2, p_message));
}

// -- Behaviour
const QStringList &Logger::getFatalErrors()
{
    return m_lFatalErrors;
}

void Logger::addFatalError(const QString & p_message)
{
    m_lFatalErrors.append(p_message);
}

void Logger::addLog(logMessage* p_log)
{
    if( m_isFinished ) {
        // treat logs after Logger destruction
        std::cerr<<p_log->m_header.toStdString()<<" - "<<p_log->m_message.toStdString()<<"\n";
    } else {
        // add to fatal list if level is FATAL
        if( p_log->m_level == logMessage::LOG_LEVEL_FATAL ) {
            addFatalError(p_log->m_message);
        }

        // add to the list if verbosity level is accepted
        if( p_log->m_level <= m_verbosity ) {
            m_mLogs.lock();
            if(m_lLogs.size() < MAX_LIST_LOG_SIZE ) {
                m_lLogs.append(p_log);
            } else {
                m_countLogLost++;
                delete p_log;
            }
            m_mLogs.unlock();
            if( nullptr != Logger::getInstance() ) {
                Logger::getInstance()->startProcessingIfNeeded();
            }
        }
    }
}

bool Logger::isRunning()
{
    return m_thLogger->isRunning();
}

// -- Inherited behaviour
bool Logger::customLoadSettings(QDomElement * p_pParent)
{
    bool bRet = true;
    bRet &= XML_READ_UNIQUE(bool, m_logFile);
    bRet &= XML_READ_UNIQUE(quint32, m_maxFileLines);
    bRet &= XML_READ_UNIQUE(quint32, m_verbosity);
    return bRet;
}

bool Logger::customLoadDefaultSettings()
{
    m_logFile = true;
    m_maxFileLines = 10000;
    m_verbosity = logMessage::LOG_LEVEL_MAX;
    return true;
}

bool Logger::customApplySettings()
{
    setLogInFile(m_logFile);
    return true;
}

bool Logger::customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent)
{
    XML_WRITE_UNIQUE(m_logFile);
    XML_WRITE_UNIQUE(m_maxFileLines);
    XML_WRITE_UNIQUE(m_verbosity);
    return true;
}

void Logger::onProcessOneLog()
{
    // process oldest log (FIFO)
    logMessage * pLog = nullptr;

    m_mLogs.lock();
    if( m_lLogs.size() ) {
        pLog = m_lLogs.first();
        m_lLogs.removeFirst();
    }
    m_mLogs.unlock();

    if( nullptr != pLog ) {
        QString vId = " ";
        if( pLog->m_level<VERBOSITY_ID_SIZE ) {
            vId = VERBOSITY_ID[pLog->m_level];
        }
        QString textLog = QString("%1 [%2] - %3").arg(pLog->m_header).arg(vId).arg(pLog->m_message);
        // Add info about missing logs :
        if( m_countLogLost ) {
            m_mLogs.lock();
            quint32 lostLogs = m_countLogLost;
            m_countLogLost = 0; // reset count of lost logs
            m_mLogs.unlock();
            textLog = QString("Lost logs : %1 !\r\n%2").arg(lostLogs).arg(textLog);
            m_fileLineCount++; // add 2 line for 1 logs
        }

        // display log in the console with colors :
        switch(pLog->m_level) {
            case logMessage::LOG_LEVEL_FATAL :
                QTextStream(stdout)<<"\e[48;5;11m\033[38;5;9m"<<textLog.toLatin1().data()<<Qt::endl;
                break;
            case logMessage::LOG_LEVEL_ERROR :
                QTextStream(stdout)<<"\e[49m\033[38;5;210m"<<textLog.toLatin1().data()<<Qt::endl;
                break;
            case logMessage::LOG_LEVEL_WARNING :
                QTextStream(stdout)<<"\e[49m\033[38;5;11m"<<textLog.toLatin1().data()<<Qt::endl;
                break;
            case logMessage::LOG_LEVEL_INFO :
                QTextStream(stdout)<<"\e[49m\033[38;5;15m"<<textLog.toLatin1().data()<<Qt::endl;
                break;
            case logMessage::LOG_LEVEL_DEBUG1 :
                QTextStream(stdout)<<"\e[49m\033[38;5;14m"<<textLog.toLatin1().data()<<Qt::endl;
                break;
            case logMessage::LOG_LEVEL_DEBUG2 :
                QTextStream(stdout)<<"\e[49m\033[38;5;10m"<<textLog.toLatin1().data()<<Qt::endl;
                break;
            default :
                QTextStream(stdout)<<"\e[49m\033[38;5;15m"<<textLog.toLatin1().data()<<Qt::endl;
                break;
        }

        // Write the log into the file
        m_mLogFile.lock();
        if( m_logFile && nullptr != m_pLogFile && m_pLogFile->isOpen() ) {
            // Add end of line :
            textLog.append("\n");
            // Write the log
            m_pLogFile->write(textLog.toLatin1());
            m_pLogFile->flush();

            // update line count :
            m_fileLineCount++;
            if( m_fileLineCount >= m_maxFileLines ) {
                rollLogFile();
            }

        }
        m_mLogFile.unlock();

        // Emit log message
        emit evNewLog(pLog->m_level, pLog->m_header, pLog->m_message);

        // Delete the log
        delete pLog;
    }

    // Any more logs to treat ?
    m_mLogs.lock();
    m_isProcessingLog = m_lLogs.size();
    m_mLogs.unlock();

    if( m_isProcessingLog ) {
        emit evProcessOneLog();
    }
}

void Logger::openLogFile()
{
    m_mLogFile.lock();
    if( m_pLogFile == nullptr ) {
        // create the file
        m_pLogFile = new QFile(m_logFileName);
        if( !m_pLogFile->open(QIODevice::ReadWrite | QIODevice::Append) ) {
            delete m_pLogFile;
            m_pLogFile = nullptr;
            logError(QString("Logger::openLogFile - Fail to open file to log : %1").arg(m_logFileName));
        } else {
            // write the header
            m_pLogFile->write(m_logHeader.toLatin1());
            // rest number of lines in the file (header is 3 lines)
            m_fileLineCount = 3;
        }
    } else {
        logError("Logger::openLogFile - File already exists");
    }
    m_mLogFile.unlock();
}

void Logger::rollLogFile()
{
    // delete last old log file
    if( QFile::exists(m_logFileNameOld) ) {
        if( !QFile::remove(m_logFileNameOld) ) {
            logError(QString("Logger::rollLogFile - Fail to remove file : %1").arg(m_logFileNameOld));
        }
    }
    // stop current log file :
    m_mLogFile.lock();
    // close current file, rename it with old log filename
    closeLogFile(m_logFileNameOld);
    m_mLogFile.unlock();
    // open new log file
    openLogFile();
}

void Logger::closeLogFile(QString p_renameFile)
{
    if( nullptr != m_pLogFile ) {
        // stop writing log in file :
        m_mLogFile.lock();
        // close file :
        if( m_pLogFile->isOpen() ) {
            m_pLogFile->flush();
            m_pLogFile->waitForBytesWritten(500);
            m_pLogFile->close();
        }

        // rename ?
        if( !p_renameFile.isEmpty() ) {
            // delete eventually the file with the new name
            QFile::remove(p_renameFile);
            // rename
            m_pLogFile->rename(p_renameFile);
        }

        delete m_pLogFile;
        m_pLogFile = nullptr;

        m_mLogFile.unlock();
    }
}

void Logger::startProcessingIfNeeded()
{
    m_mLogs.lock();
    bool bStartProcessing =  m_lLogs.size() && !m_isProcessingLog;
    if( bStartProcessing  ) {
        m_isProcessingLog = true;
    }
    m_mLogs.unlock();
    if( bStartProcessing  ) {
        emit evProcessOneLog();
    }
}


/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : WinLog.cpp
# Author      : Thierry Lechevallier
# Date        : 28/09/2020
#
# ==========================================================================*/


#include "WinLog.h"
#include "ui_WinLog.h"

#include <QScrollBar>
#include <QDebug>

WinLog::WinLog(QWidget *parent) :
    QWidget(parent), Loggable("WinLog"),
    m_pUi(new Ui::WinLog)
{
    m_pUi->setupUi(this);

    m_dispFatal = true;
    m_dispError = true;
    m_dispWarning = true;
    m_dispInfo = true;
    m_dispDebug1 = true;
    m_dispDebug2 = true;

    m_pUi->cbFatal->setChecked(m_dispFatal);
    m_pUi->cbError->setChecked(m_dispError);
    m_pUi->cbWarning->setChecked(m_dispWarning);
    m_pUi->cbInfo->setChecked(m_dispInfo);
    m_pUi->cbDebug1->setChecked(m_dispDebug1);
    m_pUi->cbDebug2->setChecked(m_dispDebug2);

    // -- List



    m_plogListModel = new logListModel();
    m_plogSortFilterProxyModel = new logSortFilterProxyModel(this);
    m_pLogListView = new logListView(m_plogListModel);
    m_pUi->glListView->addWidget(m_pLogListView);

    m_plogSortFilterProxyModel->setSourceModel(m_plogListModel);
    m_pLogListView->setModel(m_plogSortFilterProxyModel);

    connect(m_plogListModel, &logListModel::dataChanged, m_plogSortFilterProxyModel, &logSortFilterProxyModel::invalidate);
    connect(m_plogListModel, &logListModel::dataChanged, m_pLogListView, &logListView::onDataChanged);

    // -- Connections
    connect(m_pUi->cbFatal, &QCheckBox::stateChanged, this, &WinLog::onCbFatal);
    connect(m_pUi->cbError, &QCheckBox::stateChanged, this, &WinLog::onCbError);
    connect(m_pUi->cbWarning, &QCheckBox::stateChanged, this, &WinLog::onCbWarning);
    connect(m_pUi->cbInfo, &QCheckBox::stateChanged, this, &WinLog::onCbInfo);
    connect(m_pUi->cbDebug1, &QCheckBox::stateChanged, this, &WinLog::onCbDebug1);
    connect(m_pUi->cbDebug2, &QCheckBox::stateChanged, this, &WinLog::onCbDebug2);

    connect(m_pUi->pbClear, &QPushButton::clicked, this, &WinLog::onPbClear);

}


WinLog::~WinLog()
{
    delete m_plogListModel;
    delete m_plogSortFilterProxyModel;
    delete m_pLogListView;
    delete m_pUi;
}


bool WinLog::isDispFatal() const
{
    return m_dispFatal;
}

bool WinLog::isDispError() const
{
    return m_dispError;
}

bool WinLog::isDispWarning() const
{
    return m_dispWarning;
}

bool WinLog::isDispInfo() const
{
    return m_dispInfo;
}

bool WinLog::isDispDebug1() const
{
    return m_dispDebug1;
}

bool WinLog::isDispDebug2() const
{
    return m_dispDebug2;
}


void WinLog::onPbClear()
{
    m_plogListModel->clear();
}

void WinLog::onCbFatal()
{
    m_dispFatal = m_pUi->cbFatal->isChecked();
    m_plogSortFilterProxyModel->verbosityChanged();
}

void WinLog::onCbError()
{
    m_dispError = m_pUi->cbError->isChecked();
    m_plogSortFilterProxyModel->verbosityChanged();
}

void WinLog::onCbWarning()
{
    m_dispWarning = m_pUi->cbWarning->isChecked();
    m_plogSortFilterProxyModel->verbosityChanged();
}

void WinLog::onCbInfo()
{
    m_dispInfo = m_pUi->cbInfo->isChecked();
    m_plogSortFilterProxyModel->verbosityChanged();
}

void WinLog::onCbDebug1()
{
    m_dispDebug1 = m_pUi->cbDebug1->isChecked();
    m_plogSortFilterProxyModel->verbosityChanged();
}

void WinLog::onCbDebug2()
{
    m_dispDebug2 = m_pUi->cbDebug2->isChecked();
    m_plogSortFilterProxyModel->verbosityChanged();

}

// ----------------------------------------------------------------------------
// -- class LogListModel

logListModel::logListModel () : Loggable("logListModel")
{
    m_pLogger = Logger::getInstance();
    m_rowRemoved = 0;
    connect(m_pLogger, SIGNAL(evNewLog(quint32, const QString &, const QString &))
               , this, SLOT(onNewLog(quint32, const QString &, const QString &)), Qt::QueuedConnection);
}

logListModel::~logListModel()
{
    disconnect(m_pLogger, SIGNAL(evNewLog(quint32, const QString &, const QString &))
               , this, SLOT(onNewLog(quint32, const QString &, const QString &)));
    for(auto i : qAsConst(m_lLogs)) {
        delete i;
    }
    m_lLogs.clear();
}


int logListModel::rowCount(const QModelIndex & parent) const
{
    if (parent.isValid())
        return 0;
    return m_lLogs.size();
}

int logListModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant logListModel::data(const QModelIndex & index, int role) const
{
    QVariant ret;
    if( index.row() < m_lLogs.size() ) {

        if( Qt::DisplayRole == role ) {
            if( index.column() == 0 ) {
                ret = QString(m_lLogs.at(index.row())->m_header + " " + m_lLogs.at(index.row())->m_message);
            }
        } else {
            if( Qt::ForegroundRole == role ) {
                int l =  m_lLogs.at(index.row())->m_level;
                if( logMessage::LOG_LEVEL_FATAL == l ) {
                    ret = QColor(192, 0, 0);
                } else if( logMessage::LOG_LEVEL_ERROR== l ) {
                    ret = QColor(192, 0, 0);
                } else if( logMessage::LOG_LEVEL_WARNING == l ) {
                    ret = QColor(128, 128, 0);
                } else if( logMessage::LOG_LEVEL_INFO == l ) {
                    ret = QColor(128, 128, 128);
                } else if( logMessage::LOG_LEVEL_DEBUG1 == l ) {
                    ret = QColor(0, 128, 128);
                } else if( logMessage::LOG_LEVEL_DEBUG2 == l ) {
                    ret = QColor(0, 128, 0);
                }

            } else {
                if( Qt::BackgroundRole == role ) {
                    int l =  m_lLogs.at(index.row())->m_level;
                    if( logMessage::LOG_LEVEL_FATAL == l ) {
                        ret = QColor(255, 255, 0);
                    }
                }
                else {
                    if( Qt::UserRole == role ) {
                        int l =  m_lLogs.at(index.row())->m_level;
                        ret = l;
                    }
                }
            }
        }
    }

    return ret;
}

void logListModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, m_lLogs.size()-1);
    for(auto i : qAsConst(m_lLogs)) {
        delete i;
    }
    m_lLogs.clear();
    endRemoveRows();
}

int logListModel::resetRemovedRow()
{
    int ret = m_rowRemoved;
    m_rowRemoved = 0;
    return ret;
}


void logListModel::onNewLog(quint32 level, const QString &header, const QString &message)
{
    logMessage * log = new logMessage(level, message, header);
    m_lLogs.append(log);

    QModelIndex iStart = index(m_lLogs.size()-1, 0);
    QModelIndex iEnd = index(m_lLogs.size()-1, 0);

    if( m_lLogs.size() > MAX_ROW_LOG ) {
        logMessage * l = m_lLogs.first();
        m_lLogs.removeFirst();
        delete l;
        iStart = index(0,0);
        m_rowRemoved++;
    }
    emit dataChanged(iStart, iEnd);
}

// ----------------------------------------------------------------------------
// -- class LogSortFilterProxyModel

logSortFilterProxyModel::logSortFilterProxyModel(WinLog * p_WinLog) : QSortFilterProxyModel(), Loggable("logSortFilterProxyModel")
{
    m_pWinLog = p_WinLog;
}


logSortFilterProxyModel::~logSortFilterProxyModel()
{

}

void logSortFilterProxyModel::verbosityChanged()
{
    invalidateFilter();
}

bool logSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    bool bRet = false;

    QModelIndex i = sourceModel()->index(sourceRow, 0, sourceParent);
    int l = sourceModel()->data(i, Qt::UserRole).toInt();

    if( logMessage::LOG_LEVEL_FATAL == l && m_pWinLog->isDispFatal() ) {
        bRet = true;
    } else if( logMessage::LOG_LEVEL_ERROR == l && m_pWinLog->isDispError() ) {
        bRet = true;
    } else if( logMessage::LOG_LEVEL_WARNING == l && m_pWinLog->isDispWarning() ) {
        bRet = true;
    } else if( logMessage::LOG_LEVEL_INFO == l && m_pWinLog->isDispInfo() ) {
        bRet = true;
    } else if( logMessage::LOG_LEVEL_DEBUG1 == l && m_pWinLog->isDispDebug1() ) {
        bRet = true;
    } else if( logMessage::LOG_LEVEL_DEBUG2 == l && m_pWinLog->isDispDebug2() ) {
        bRet = true;
    }

    return bRet;
}

// ----------------------------------------------------------------------------
// -- class logListView


logListView::logListView(logListModel *p_pLogListModel) : Loggable("logListView")
{
        m_pLogListModel = p_pLogListModel;
        m_scrollMode = 0;
        scrollToBottom();
        m_tAutoScroll = new QTimer();

        connect(m_tAutoScroll, &QTimer::timeout, this, &logListView::onAutoscrollTimeout);
        connect(this, &logListView::clicked, this, &logListView::onPressed);
        connect(verticalScrollBar(), &QScrollBar::sliderMoved, this, &logListView::onVerticalScroll);        

        m_tAutoScroll->setSingleShot(true);
        scrollToBottom();
        setAutoScroll(true);
        setSelectionMode(QAbstractItemView::SingleSelection);

        setFont(QFont("Consolas", 10));

}


logListView::~logListView()
{

}

void logListView::onDataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)
{
    if( 0 == m_scrollMode ) {
        onAutoscrollTimeout();
    } else {
        m_iScroll -= m_pLogListModel->resetRemovedRow();
        if( m_iScroll < 0 ) {
            m_iScroll = 0;
        }
        QModelIndex index = model()->index(m_iScroll,0);
        scrollTo(index);
        setCurrentIndex(index);
        //qDebug()<<"scroll to "<<m_iScroll;
    }
}

void logListView::onVerticalScroll(int pos)
{
    if( pos > (verticalScrollBar()->maximum())*95/100) {
        // cursor on bottom
        onAutoscrollTimeout();
    } else {
        startScrollTimer();
        m_scrollMode = 2;
    }
}

void logListView::onPressed(const QModelIndex &index)
{
    m_iScroll = index.row();
    m_pLogListModel->resetRemovedRow();
    scrollTo(index);
    setCurrentIndex(index);
    m_scrollMode = 1;
    startScrollTimer();
}

void logListView::onAutoscrollTimeout()
{
    if( m_tAutoScroll->isActive() ) {
        m_tAutoScroll->stop();
    }

    if( 0 != m_scrollMode ) {
        m_scrollMode = 0;
        scrollToBottom();
    } else {
        verticalScrollBar()->setValue(verticalScrollBar()->maximum());
    }
}

void logListView::startScrollTimer()
{
    if( m_tAutoScroll->isActive() ) {
        m_tAutoScroll->stop();
    }
    m_tAutoScroll->start(TIME_AUTO_SCROLL_BOTTOM);
}

/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : WinLog.h
# Author      : Thierry Lechevallier
# Date        : 28/09/2020
#
# ==========================================================================*/

#pragma once

// -- Qt includes
#include <QWidget>
#include <QSortFilterProxyModel>
#include <QListView>
#include <QTimer>

// -- Internal includes
#include <log/Loggable.h>
#include <log/Logger.h>

class logSortFilterProxyModel;
class logListModel;
class logListView;

namespace Ui {
class WinLog;
}

// ----------------------------------------------------------------------------
// -- class WinLog

class WinLog : public QWidget, public Loggable
{
    Q_OBJECT

public:
    explicit WinLog(QWidget *parent = nullptr);
    ~WinLog();

    bool isDispFatal() const;
    bool isDispError() const;
    bool isDispWarning() const;
    bool isDispInfo() const;
    bool isDispDebug1() const;
    bool isDispDebug2() const;


private slots :
    void onPbClear();
    void onCbFatal();
    void onCbError();
    void onCbWarning();
    void onCbInfo();
    void onCbDebug1();
    void onCbDebug2();


private:
    Ui::WinLog *m_pUi;

    bool m_dispFatal;
    bool m_dispError;
    bool m_dispWarning;
    bool m_dispInfo;
    bool m_dispDebug1;
    bool m_dispDebug2;

    logListModel * m_plogListModel;
    logSortFilterProxyModel * m_plogSortFilterProxyModel;
    logListView * m_pLogListView;
};


// ----------------------------------------------------------------------------
// -- class LogListModel

class logListModel : public QAbstractListModel, public Loggable
{
    Q_OBJECT

public :
    explicit logListModel();
    ~logListModel();


    Logger * m_pLogger;
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

    void clear();

    static const qint32 MAX_ROW_LOG = 1000;

    int resetRemovedRow(void);


private slots :
    void onNewLog(quint32 level, const QString &header, const QString & message);

private:

    int m_rowRemoved;
    QList<logMessage *> m_lLogs;
};

// ----------------------------------------------------------------------------
// -- class LogSortFilterProxyModel

class logSortFilterProxyModel : public QSortFilterProxyModel, public Loggable
{
    Q_OBJECT

public:
    logSortFilterProxyModel(WinLog * p_WinLog);
    ~logSortFilterProxyModel();

    void verbosityChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private :
    WinLog * m_pWinLog;
};

// ----------------------------------------------------------------------------
// -- class LogListView

class logListView : public QListView, public Loggable
{
    Q_OBJECT

public:
    logListView(logListModel * p_pLogListModel);
    ~logListView();

public slots :
    void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

private slots :
    void onVerticalScroll(int);
    void onPressed(const QModelIndex &index);
    void onAutoscrollTimeout();

private :

    void startScrollTimer();

    int m_scrollMode;   // 0 = bottom, 1 = item
    qint32 m_iScroll;

    static const qint32 TIME_AUTO_SCROLL_BOTTOM = 10000;    // 10 sec

    QTimer * m_tAutoScroll;
    logListModel * m_pLogListModel;


};

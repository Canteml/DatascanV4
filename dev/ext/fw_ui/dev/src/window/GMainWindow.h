/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : GMainWindow.h
# Author      : Thierry Lechevallier
# Date        : 30/07/2020
#
# ==========================================================================*/

#pragma once

#include <QMainWindow>
#include <xml/XmlSettings.h>
#include <log/Loggable.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class GMainWindow : public QMainWindow, public XmlSettings, public Loggable
{
    Q_OBJECT

public:
    GMainWindow(QWidget *parent = nullptr);
    virtual ~GMainWindow();

    bool initMainWindow();  ///< call m_pUi->setup() then initCustomMainWindow

    virtual QString getWindowTitle(void); ///< title of the window

protected :
    //-- Behaviour
    virtual bool initCustomMainWindow(void) = 0;    // inherited class

    // -- Override QMainWindow
    virtual void closeEvent(QCloseEvent *event) override;

    // -- XmlSettings : Inherited behaviour
    virtual bool customLoadSettings(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultSettings() override;
    virtual bool customApplySettings() override;
    virtual bool customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

protected:
    Ui::MainWindow * m_pUi;

    QString m_geometryAtClose;
    QString m_stateAtClose;
};

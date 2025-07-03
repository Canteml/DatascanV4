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

// -- Qt includes
#include <QRect>
class QApplication;
class GMainWindow;

// -- Internal includes
#include <core/Core.h>
#include <xml/XmlSettings.h>

// ----------------------------------------------------------------------------
// -- Class CoreUiWidget
/**
 * @brief The CoreUiWidget class
 *
 */
class CoreUiWidget : public QObject, public Core, public XmlSettings
{
    Q_OBJECT
public :
    CoreUiWidget(const QString& p_coreName);       ///< public constructor to be inherited
    virtual ~CoreUiWidget();
    static CoreUiWidget * getInstanceCoreUiWidget();    ///< get core instance

    // -- Behaviour
    int execUiWidget(int argc, char* argv[]);

    static QString getDirAppResQml();  ///< Resources qml

protected :
    // -- Behaviour

    // Custom behaviour for inherited classes (implemented empty)
    virtual bool initApp() override;     ///< Make managers
    virtual bool startApp() override;    ///< Start managers
    virtual void stopApp() override;     ///< Stop managers
    virtual void deleteApp() override;   ///< Delete managers

    virtual void help(QStringList & p_help) const override;    ///< inherited class can add lines to the help

    virtual GMainWindow * makeMainWindow() = 0;     ///< inherited class must return its mainwindow

protected :
    // -- XmlSettings : Inherited behaviour
    virtual bool customLoadSettings(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultSettings() override;
    virtual bool customApplySettings() override;
    virtual bool customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent) override;


    // -- Attributs
    // Behaviour
    static CoreUiWidget * m_pCoreUiWidgetInstance;

private :
    QApplication * m_pApplication;
    GMainWindow * m_pCoreMainVindow;

};

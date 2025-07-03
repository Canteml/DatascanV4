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
#include <QUrl>
class QGuiApplication;
class QApplication;
#include <QWindow>
class QQmlApplicationEngine;
class QQuickWindow;

// -- Internal includes
#include <core/Core.h>
#include <xml/XmlSettings.h>

// ----------------------------------------------------------------------------
// -- Class CoreUiQml
/**
 * @brief The CoreUiQml class
 *
 * - Add path res/qml to access to qml files
 * - Add interface for QML init (path, RegisterType)
 */
class CoreUiQml : public QObject, public Core, public XmlSettings
{
    Q_OBJECT
public :
    CoreUiQml(const QString& p_coreName);       ///< public constructor to be inherited
    virtual ~CoreUiQml();
    static CoreUiQml * getInstanceCoreUiQml();    ///< get core instance

    // -- Behaviour
    int execUiQml(int argc, char* argv[]);
    int execUiQml_withSelectLanguage(int argc, char* argv[]);
    int execUiQml_EnglishLanguage(int argc, char* argv[]);
    int execUiQml_EspanolLanguage(int argc, char* argv[]);

    static QString getDirAppQml();  ///< Resources qml

    virtual QString getWindowTitle(void) const;   ///< default is "APP - Version"

    QQmlApplicationEngine *getApplicationEngine() const;

public slots :
    void updateWindowTitle(void);

protected :
    // -- Behaviour
    virtual bool eventFilter(QObject*, QEvent*) override;   ///< key catcher

    // Custom behaviour for inherited classes (implemented empty)
    virtual bool initApp() override;     ///< Make managers
    virtual bool startApp() override;    ///< Start managers
    virtual void stopApp() override;     ///< Stop managers
    virtual void deleteApp() override;   ///< Delete managers

    virtual bool initQmlType() = 0;
    virtual bool initQmlContext() = 0;
    virtual bool initQmlPath() = 0;
    virtual void initCppFromQml();      ///< called once GUI is instanciated. m_pQmlTopLevel is valid

    virtual void help(QStringList & p_help) const override;    ///< inherited class can add lines to the help
    virtual bool useOpenGlRender() const;


protected slots :
    void onQmlCreated(QObject *object, const QUrl &url);
    void onWinVisibilityChanged(QWindow::Visibility);
    void onReloadQml();

signals :
    void evReloadQml();

protected :
    // -- XmlSettings : Inherited behaviour
    virtual bool customLoadSettings(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultSettings() override;
    virtual bool customApplySettings() override;
    virtual bool customSaveSettings(QDomDocument * p_pDoc, QDomElement * p_pParent) override;


    // -- Attributs
    // Behaviour
    static CoreUiQml * m_pCoreUiQmlInstance;
    QUrl m_mainQmlUrl;
    QString m_mainQmlFile;

    QGuiApplication * m_pGuiApplication;
    QApplication * m_pApplication;
    QQmlApplicationEngine * m_pQmlApplicationEngine;
    QQuickWindow * m_pMainView;
    QObject * m_pQmlTopLevel;

    // Settings
    quint32 m_settWinVisibility;
    QRect m_settWinGeometry;
};

// ----------------------------------------------------------------------------
// -- Class KeyCatcher

class KeyCatcher : public QObject
{
    Q_OBJECT
public :
    KeyCatcher();


};

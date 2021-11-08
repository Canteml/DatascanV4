/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : GlobalManager.h
# Author      : Thierry Lechevallier
# Date        : 02/09/2021
#
# ==========================================================================*/

#pragma once

#include <log/Loggable.h>
class ExamManager;
class ComDeviceManager;

/**
 * @brief The GlobalManager class
 *
 * What this name for a class ?!!
 * Here, we will manage all global behaviour. No settings, only volatile data
 *  - GUI dynamic configuration (link qml/c++)
 *      => which page is displayed ?
 */
class GlobalManager : public QObject, public Loggable
{
    Q_OBJECT
    Q_PROPERTY(quint32 guiPage READ getGuiPage WRITE setGuiPage NOTIFY evGuiPageChanged)
    Q_PROPERTY(bool keyboardQwerty READ getKeyboardQwerty NOTIFY evKeyboardQwertyChanged)

public:    

    /**
     * @brief The GUI_PAGE enum
     * Page displayed in central GUI
     */
    enum GUI_PAGE{
        PAGE_PROJECT,
        PAGE_PREPARE,
        PAGE_SUPERVISE,
        PAGE_SCAN,
        PAGE_HELP
    };

    ~GlobalManager();

    static GlobalManager* makeInstance();
    void initManager();

    static GlobalManager* getInstance();

    quint32 getGuiPage() const;
    void setGuiPage(quint32 p_value);

    bool getKeyboardQwerty() const;


signals :
    void evGuiPageChanged(quint32);
    void evKeyboardQwertyChanged(bool);

private slots :
    void onExamManagerStepChanged(qint32);
    void onDeviceListChanged();

private :
    GlobalManager();

    // -- Properties
    quint32 m_guiPage; //    project : 0 / prepare : 1 / sueprvise : 2 / scan : 3 / help : 4

    // -- Behaviour
    static GlobalManager* m_pMyInstance;
    ExamManager* m_pExamManager;
    bool m_isKeyboardQwerty;

    ComDeviceManager* m_pComDeviceManager;
};


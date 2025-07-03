/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : ProjectDate.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlProject.h>
#include <QDateTime>

// ----------------------------------------------------------------------------
// -- Class ProjectDate

/**
 * @brief the ProjectDate class
 *
 * Just a base to get a date and convenient methods to set them.
 *
 */
 
class ProjectDate : public QObject, public XmlProjectLoggable
{
    Q_OBJECT
public :
    explicit ProjectDate(const QString & p_className = "ProjectDate");
    explicit ProjectDate(const ProjectDate & p_value);
    virtual ~ProjectDate();
    
    QDateTime getDateTime() const;
    QDate getDate() const;
    QString getDateString() const;
    void setDate(const QDateTime & p_value);
    void setDate(const QDate & p_value);
    void setDate(int p_hour, int p_minute, int p_second, int p_day, int p_month, int p_year);
    void setDate(int p_day, int p_month, int p_year);
    void setDateToNow(void);
    bool isDateSet() const;

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

    // -- operator
    ProjectDate & operator= (const ProjectDate & p_value);
    bool operator== (const ProjectDate & p_value);
    
protected :
    
    // -- Params
    QString m_date; // if empty, then date is nopt set

};

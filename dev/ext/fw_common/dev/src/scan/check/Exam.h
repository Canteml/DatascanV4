/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Exam.h
# Author      : Thierry Lechevallier
# Date        : 27/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlProject.h>
#include <scan/type/Report.h>

class Structure;
class Operator;
class Equipment;
class CheckPiece;

class ExamReport;

// -- Qt includes
#include <QDateTime>
#include <QList>


// ----------------------------------------------------------------------------
// -- Class Exam

/**
 * @brief the Exam class
 *
 */
 
class Exam : public QObject, public XmlProjectLoggable
{
    Q_OBJECT
public :
    explicit Exam();
    virtual ~Exam();

    // -- Behaviour
    void prepareCheck();        ///< reset reports, delete items of m_lCheckPiece then build m_lCheckPiece function to m_pStructure

    // -- Properties
    QDateTime getDate() const;
    QString getDateString() const;
    void setDate(const QDateTime & p_value);
    void setDate(int p_hour, int p_minute, int p_second, int p_day, int p_month, int p_year);
    void setDateToNow(void);
    bool isDateSet() const;
    QString getComment() const;
    void setComment(const QString & p_value);

    // -- CheckPiece

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;
    
protected :
    
    void removeAllCheckPiece(void);

    // -- Properties
    QString m_date;     // if empty, then date is not set
    QString m_comment;

    // -- Data
    Structure * m_pStructure;
    Operator * m_pOperator;
    Equipment * m_pEquipment;
    QList<CheckPiece *> m_lCheckPiece;

    ExamReport * p_ExamReport;
};


// ----------------------------------------------------------------------------
// -- Class ExamReport

/**
 * @brief The ExamReport class
 *
 */

class ExamReport : public Report
{
public :
    ExamReport();
    ~ExamReport();


protected :
};

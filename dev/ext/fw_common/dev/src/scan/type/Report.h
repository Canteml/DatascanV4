/*============================================================================
#
#   Copyright Roboplanet 2020
#
# ----------------------------------------------------------------------------
#
# File name   : Report.h
# Author      : Thierry Lechevallier
# Date        : 06/10/2020
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <scan/type/ProjectDate.h>

// ----------------------------------------------------------------------------
// -- Class Report

/**
 * @brief the Report class
 *
 * Just a base to get a date and convenient methods to set them.
 *
 */

class Report : public ProjectDate
{

public :

    enum ReportStatus {
        REPORT_UNKNOW,
        REPORT_KO,
        REPORT_OK
    };

    explicit Report(const QString & p_className = "Report");
    explicit Report(const Report & p_value);
    virtual ~Report();

    // -- Params
    quint32 getStatus(void) const;
    void setStatus(quint32 p_value);
    qreal getValueMin(void) const;
    void setValueMin(qreal p_value);
    qreal getValueMax(void) const;
    void setValueMax(qreal p_value);

    // -- XmlProject
    virtual bool customLoadParam(QDomElement * p_pParent) override;
    virtual bool customLoadDefaultParam() override;
    virtual bool customApplyParam() override;
    virtual bool customSaveParam(QDomDocument * p_pDoc, QDomElement * p_pParent) override;

protected :

    // -- Params
    quint32 m_status;   ///< value in ReportStatus
    qreal m_valueMin;
    qreal m_valueMax;
};

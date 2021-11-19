/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ExamPipe.h
# Author      : Thierry Lechevallier
# Date        : 17/11/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include "log/Loggable.h"

// ----------------------------------------------------------------------------
// -- ExamPipeSensor

/**
 * @brief The ExamPipeSensor class
 *
 * One ExamPipeSensor associate one measure with one picture
 *
 */

class ExamPipeSensor : public Loggable
{
public :
    ExamPipeSensor();
    ~ExamPipeSensor();

    QString m_photoFileName;
    qreal m_measureMm;          ///< -1 = no measure done

    static const int SENSOR_NO_MEARURE = -1;
};


// ----------------------------------------------------------------------------
// -- ExamCircular

/**
 * @brief The ExamCircular class
 *
 * One ExamCircular associate 4 sensors with one comment
 *
 */

class ExamCircular : public Loggable
{
public:
    ExamCircular();
    ~ExamCircular();

    QString m_comment;
    ExamPipeSensor m_aExamPipeSensor[4];   ///< 4 sensors
};


// ----------------------------------------------------------------------------
// -- ExamPipe

/**
 * @brief The ExamPipe class
 *
 * One ExamPipe is a list of ExamCircular
 *
 */

class ExamPipe : public Loggable
{
public:
    ExamPipe();
    ~ExamPipe();

    void clear();

    QList<ExamCircular* > m_lExamCircular;
};

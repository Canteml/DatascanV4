/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ExamPipe.cpp
# Author      : Thierry Lechevallier
# Date        : 17/11/2021
#
# ==========================================================================*/

// -- Internal includes
#include "ExamPipe.h"


// ----------------------------------------------------------------------------
// -- ExamPipeSensor

ExamPipeSensor::ExamPipeSensor() : Loggable("ExamPipeSensor")
{
    m_measureMm = -1;
}

ExamPipeSensor::~ExamPipeSensor()
{

}

// ----------------------------------------------------------------------------
// -- ExamCircular

ExamCircular::ExamCircular() : Loggable("ExamCircular")
{

}

ExamCircular::~ExamCircular()
{

}

// ----------------------------------------------------------------------------
// -- ExamPipe

ExamPipe::ExamPipe() : Loggable("ExamPipe")
{

}

ExamPipe::~ExamPipe()
{
    clear();
}

void ExamPipe::clear()
{
    while(m_lExamCircular.size()) {
        delete m_lExamCircular.takeLast();
    }
}

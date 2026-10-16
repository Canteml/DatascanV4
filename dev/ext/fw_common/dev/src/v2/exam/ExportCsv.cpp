/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ExportCsv.cpp
# Author      : Thierry Lechevallier
# Date        : 29/09/2021
#
# ==========================================================================*/

// -- Internal includes
#include <v2/exam/ExportCsv.h>
#include <v2/scan/AScan.h>

#include <v2/vector/VectorManager.h>
#include <v2/vector/VectorStd.h>
#include <v2/vector/VectorFuncAScan.h>
#include <v2/vector/VectorFuncLicence.h>
#include <v2/vector/VectorFuncTemperatureSensor.h>
#include <v2/vector/VectorFuncHumiditySensor.h>

// -- Qt Includes
#include <QDateTime>

ExportCsv::ExportCsv() : Loggable("ExportCsv")
{
    m_pThread = new QThread();
    connect(this, &ExportCsv::evPrivateOpen, this, &ExportCsv::onPrivateOpen, Qt::QueuedConnection);
    connect(this, &ExportCsv::evPrivateWriteLine, this, &ExportCsv::onPrivateWriteLine, Qt::QueuedConnection);
    connect(this, &ExportCsv::evPrivateClose, this, &ExportCsv::onPrivateClose, Qt::QueuedConnection);
    this->moveToThread( m_pThread );
    m_pThread->start();
}

ExportCsv::~ExportCsv()
{
    if( m_pThread->isRunning() ) {
        m_pThread->quit();
        m_pThread->wait(500);
    }
    // thread is stopped : safe to close the file from here
    onPrivateClose();
}

void ExportCsv::open(const QString& p_fileName)
{
    m_fileName = p_fileName;
    emit evPrivateOpen(p_fileName);
}

void ExportCsv::appendAScan(AScan* p_pAScan)
{
    // The line is formatted in the caller thread : the ASCAN may be deleted as soon as this function returns.
    VectorStd* pVector = VectorManager::getInstance()->getVectorStd();
    VectorFuncAScan* pVectAScan = pVector->getFuncAScan(0);

    QString line;
    QTextStream flux(&line);

    flux<<p_pAScan->getAScanRawSample()->m_position.x()/10<<";";  /// mm => cm
    flux<<p_pAScan->getAScanMeasure()->m_thicknessMmMultiEcho<<";";
    flux<<p_pAScan->getAScanMeasure()->m_thicknessMm1Echo<<";";

    // Zones (viroles) are only known at the end of the acquisition : not written in streaming
    flux<<"0;";     // Debut de la virole
    flux<<"0;";     // Fin de la virole
    flux<<"0;";     // Maximum d'épaisseur (renseigné dans DataReport)
    flux<<"0;";     // Minimum d'épaisseur (renseigné dans DataReport)
    flux<<"0;";     // Maximum d'épaisseur 1 Echo (renseigné dans DataReport)
    flux<<"0;";     // Minimum d'épaisseur 1 Echo (renseigné dans DataReport)
    flux<<"0;";     // Moyenne d'épaisseur (calculée par DataReport)
    flux<<"0;";     // Moyenne d'épaisseur 1 Echo (calculée par DataReport)
    flux<<"0;";     // Indice du minimum (renseigné dans DataReport)
    flux<<"0;";     // Indice du minimum 1E (renseigné dans DataReport)
    flux<<"0;";     // Indice du maximum 1E (renseigné dans DataReport)
    flux<<"0;";     // Indice du maximum (renseigné dans DataReport)

    flux<<QDateTime::fromMSecsSinceEpoch(p_pAScan->getAScanRawSample()->m_epocTimeMs).toString("dd.MM.yyyy hh:mm:ss.zzz")<<";";

    flux<<pVectAScan->getParamDGain()<<";";
    flux<<false/*m->isMarqueur*/<<";";
    flux<<pVectAScan->getParamLLowPassFilter()/*m->attenuation => Ordre du filtre*/<<";";
    flux<<pVectAScan->getParamLCelerity()<<";";
    flux<<p_pAScan->mmToIndex(pVectAScan->getParamLProbeDelayMm())/2<<";";
    flux<<pVectAScan->getParamDSampleFreq()<<";";
    flux<<pVectAScan->getParamDVoltage()<<";";
    flux<<pVectAScan->getParamDPulseWidth()<<";";
    flux<</*m->seuilBruit*/pVectAScan->getParamLGate1Height()<<";";
    flux<</*<m->debutSig*/p_pAScan->mmToIndex(pVectAScan->getParamLGate1Start())<<";";
    flux<</*m->finSig*/p_pAScan->mmToIndex(pVectAScan->getParamLGate1Start() + pVectAScan->getParamLGate1Width())<<";";
    flux<</*m->debut1Pic*/p_pAScan->mmToIndex(pVectAScan->getParamLGate0Start())<<";";
    flux<</*<m->fin1Pic*/p_pAScan->mmToIndex(pVectAScan->getParamLGate0Start() + pVectAScan->getParamLGate0Width())<<";";
    flux<</*m->seuil1Pic*/pVectAScan->getParamLGate0Height()<<";";

    // convert US-SPI signal to US-KEY range
    for(int j=0;j<AScanRawSample::ASCAN_SAMPLE_SIZE;j++)
    {
        qreal d = p_pAScan->getAScanRawSample()->m_rawSamples[j];
        qreal uskeyData = 2048 + ((d-127)*10);
        flux<<(unsigned short)(uskeyData)<<";";
    }

    // temperature / humidity at acquisition time ("-" if the sensor is not enabled)
    if( pVector->funcTemperatureSensor0()->getIsEnabled() ) {
        flux << pVector->funcTemperatureSensor0()->getParamDTemperatureC() / 10.0 << ";";
    } else {
        flux << "-;";
    }
    if( pVector->funcHumiditySensor0()->getIsEnabled() ) {
        flux << pVector->funcHumiditySensor0()->getParamDHumidityC() / 10.0 << ";";
    } else {
        flux << "-;";
    }

    //flux<<m->deepCm<<";"; // IP68
    flux<<"\n";
    flux.flush();

    emit evPrivateWriteLine(line);
}

void ExportCsv::close()
{
    emit evPrivateClose();
}

QString ExportCsv::getFileName() const
{
    return m_fileName;
}

void ExportCsv::onPrivateOpen(QString p_fileName)
{
    // close a previous file if any
    onPrivateClose();

    VectorStd* pCurrentVector = VectorManager::getInstance()->getVectorStd();
    //if( (nullptr != pCurrentVector) && (nullptr != pCurrentVector->funcLicence()) ) {
    if(false && 0 == pCurrentVector->funcLicence()->getParamDStatusLicenceP() ) {
        fLogError("Licence is expired ! Can't export CSV");
        emit evError(p_fileName);
        return;
    }

    m_file.setFileName(p_fileName);
    if( !m_file.open(QIODevice::WriteOnly) ) {
        fLogError("Fail to create report file : " + p_fileName);
        emit evError(p_fileName);
        return;
    }
    m_flux.setDevice(&m_file);

    /// ---- Code imported from DatascanV4.0
    // -- Write header
    // Le fichier créé a la même forme qu'un fichier qui a déjà été modifié par DataReport, ce qui lui permet d'être lu par DataReport V3.2
    if(QString(APP_VERSION).contains("ENG"))
    {
        m_flux<<"Position;Measure;Measure1;StartPlate;EndPlate;MaxPlate;MinPlate;MaxPlate1E;MinPlate1E;AvgPlate;AvgPlate1E;IndMinPlate;IndMinPlate1E;IndMaxPlate1E;IndMaxPlate;Date;Gain;IsMarqueur;attenuation;celerity;phaseShift;SamplingFreq;voltage;PulsWidth;NoiseThreshold;StartSignal;EndSignal;StartGate1;EndGate1;ThresholdGate1;RawSignal\n";
    }
    else
    {
        m_flux<<"Position;Mesure;Mesure1;viroleDebut;viroleFin;viroleMax;viroleMin;viroleMax1E;viroleMin1E;viroleMoy;viroleMoy1E;viroleIndMin;viroleIndMin1E;viroleIndMax1E;viroleIndMax;Date;gain;isMarqueur;attenuation;célérité;déphasage;fréquenceEch;tension;largeur_puls;seuilBruit;debut_Sig;fin_Sig;deb1P;fin1P;seuil1P;Signal brut\n";
    }
    m_flux.flush();
    fLogInfo("CSV file created : " + p_fileName);
}

void ExportCsv::onPrivateWriteLine(QString p_line)
{
    if( m_file.isOpen() ) {
        m_flux << p_line;
        m_flux.flush();     // line is on disk as soon as it is measured
        if (m_flux.status() != QTextStream::Ok)
        {
            fLogError(QString("Fail to write CSV File %1").arg(m_file.fileName()));
            m_flux.resetStatus();
            emit evError(m_file.fileName());
        }
    }
}

void ExportCsv::onPrivateClose()
{
    if( m_file.isOpen() ) {
        m_flux.flush();
        m_flux.setDevice(nullptr);
        m_file.close();
        fLogInfo("CSV file closed : " + m_file.fileName());
    }
}

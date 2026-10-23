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
#include <v2/scan/Surface1d.h>
#include <v2/scan/Location1d.h>
#include <v2/exam/ExamManager.h>
#include <v2/scan/Zone1d.h>
#include <v2/scan/AScan.h>
#include <v2/scan/SurfaceCell.h>

#include <v2/vector/VectorManager.h>
#include <v2/vector/VectorStd.h>
#include <v2/vector/VectorFuncAScan.h>
#include <v2/vector/VectorFuncLicence.h>

// -- Qt Includes
#include <QDateTime>
#include <QFile>

ExportCsv::ExportCsv() : Loggable("ExportCsv")
{
    m_pThread = new QThread();
    connect(this, &ExportCsv::evPrivateStartExport, this, &ExportCsv::onPrivateStartExport, Qt::QueuedConnection);
    this->moveToThread( m_pThread );
    m_pThread->start();
}

ExportCsv::~ExportCsv()
{
    m_requestCancel = true;
    if( m_pThread->isRunning() ) {
        m_pThread->quit();
        m_pThread->wait(500);
    }
}

void ExportCsv::startExport(const QString& p_fileName)
{
    m_fileName = p_fileName;
    m_requestCancel = false;
    VectorStd* pCurrentVector = VectorManager::getInstance()->getVectorStd();
    //if( (nullptr != pCurrentVector) && (nullptr != pCurrentVector->funcLicence()) ) {
    if(false && 0 == pCurrentVector->funcLicence()->getParamDStatusLicenceP() ) {
        fLogError("Licence is expired ! Can't export CSV");
        emit evProgessChanged(-1);
        emit evExportTerminated(false);
        return;
    }
    emit evProgessChanged(0);
    emit evPrivateStartExport();
}

void ExportCsv::cancelExport()
{
    m_requestCancel = true;
}

QString ExportCsv::getFileName() const
{
    return m_fileName;
}

void ExportCsv::onPrivateStartExport()
{
    bool bOk = true;

    ExamManager * pExamManager = ExamManager::getInstance();
    Location1d* pLocation = pExamManager->getCurrentBScanGeneratrix();
    Surface1d* pSurface = pLocation->getSurface(0); // for now, write only BSCAN of sensor 0
    QVector<SurfaceCell *> * pCell = pSurface->getCell();
    const QList<Zone1d*>* pZone = pLocation->getZones();
    int cptZone = 0;
    int progress = 0;
    int lastPrecent = 0;

    VectorFuncAScan* pVectAScan = VectorManager::getInstance()->getVectorStd()->getFuncAScan(0);


    // -- Create file

    QFile file(m_fileName);
    QTextStream flux(&file);
    if(file.open(QIODevice::WriteOnly))
    {
        /// ---- Code imported from DatascanV4.0
        // -- Write header
        // Le fichier créé a la même forme qu'un fichier qui a déjà été modifié par DataReport, ce qui lui permet d'être lu par DataReport V3.2
        if(QString(APP_VERSION).contains("ENG"))
        {
            flux<<"Position;Measure;Measure1;StartPlate;EndPlate;MaxPlate;MinPlate;MaxPlate1E;MinPlate1E;AvgPlate;AvgPlate1E;IndMinPlate;IndMinPlate1E;IndMaxPlate1E;IndMaxPlate;Date;Gain;IsMarqueur;attenuation;celerity;phaseShift;SamplingFreq;voltage;PulsWidth;NoiseThreshold;StartSignal;EndSignal;StartGate1;EndGate1;ThresholdGate1;RawSignal\n";
        }
        else
        {
            flux<<"Position;Mesure;Mesure1;viroleDebut;viroleFin;viroleMax;viroleMin;viroleMax1E;viroleMin1E;viroleMoy;viroleMoy1E;viroleIndMin;viroleIndMin1E;viroleIndMax1E;viroleIndMax;Date;gain;isMarqueur;attenuation;célérité;déphasage;fréquenceEch;tension;largeur_puls;seuilBruit;debut_Sig;fin_Sig;deb1P;fin1P;seuil1P;Signal brut\n";
        }



        for(auto i : qAsConst(*pCell)) {
            AScan* pAScan = i->getAScanReference();
            if( nullptr != pAScan ) {
                flux<<pAScan->getAScanRawSample()->m_position.x()/10<<";";  /// mm => cm
                flux<<pAScan->getAScanMeasure()->m_thicknessMmMultiEcho<<";";
                flux<<pAScan->getAScanMeasure()->m_thicknessMm1Echo<<";";

                if(cptZone < pZone->size())
                {
                    qreal zoneBegin, zoneEnd;
                    Zone1d * pZone1d = pZone->at(cptZone);
                    zoneBegin = pZone1d->getOriginXMm();
                    if( 0 != pZone1d->getSizeXMm() ) {
                        zoneEnd = zoneBegin + pZone1d->getSizeXMm();
                    } else {
                        if( (cptZone + 1) < pZone->size() ) {
                            zoneEnd = pZone->at(cptZone+1)->getOriginXMm();
                        } else {
                            fLogWarning(QString("Invalid Zone : index %1/%2 , origin %3").arg(cptZone).arg(pZone->size()).arg(zoneBegin));
                            zoneEnd = zoneBegin;
                        }
                    }

                    flux<<zoneBegin<<";";
                    flux<<zoneEnd<<";";

                    flux<<"-10000;";     // Maximum d'épaisseur (renseigné dans DataReport)
                    flux<<"10000;";     // Minimum d'épaisseur (renseigné dans DataReport)
                    flux<<"-10000;";     // Maximum d'épaisseur 1 Echo (renseigné dans DataReport)
                    flux<<"10000;";     // Minimum d'épaisseur 1 Echo (renseigné dans DataReport)
                    flux<<"0;";     // Moyenne d'épaisseur (calculée par DataReport)
                    flux<<"0;";     // Moyenne d'épaisseur 1 Echo (calculée par DataReport)
                    flux<<"0;";     // Indice du minimum (renseigné dans DataReport)
                    flux<<"0;";     // Indice du minimum 1E (renseigné dans DataReport)
                    flux<<"0;";     // Indice du maximum 1E (renseigné dans DataReport)
                    flux<<"0;";     // Indice du maximum (renseigné dans DataReport)
                    cptZone++;
                }else{
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
                }
                flux<<QDateTime::fromMSecsSinceEpoch(pAScan->getAScanRawSample()->m_epocTimeMs).toString("dd.MM.yyyy hh:mm:ss.zzz")<<";";

                flux<<pVectAScan->getParamDGain()<<";";
                flux<<false/*m->isMarqueur*/<<";";
                flux<<pVectAScan->getParamLLowPassFilter()/*m->attenuation => Ordre du filtre*/<<";";
                flux<<pVectAScan->getParamLCelerity()<<";";
                flux<<pAScan->mmToIndex(pVectAScan->getParamLProbeDelayMm())/2<<";";
                flux<<pVectAScan->getParamDSampleFreq()<<";";
                flux<<pVectAScan->getParamDVoltage()<<";";
                flux<<pVectAScan->getParamDPulseWidth()<<";";
                flux<</*m->seuilBruit*/pVectAScan->getParamLGate1Height()<<";";
                flux<</*<m->debutSig*/pAScan->mmToIndex(pVectAScan->getParamLGate1Start())<<";";
                flux<</*m->finSig*/pAScan->mmToIndex(pVectAScan->getParamLGate1Start() + pVectAScan->getParamLGate1Width())<<";";
                flux<</*m->debut1Pic*/pAScan->mmToIndex(pVectAScan->getParamLGate0Start())<<";";
                flux<</*<m->fin1Pic*/pAScan->mmToIndex(pVectAScan->getParamLGate0Start() + pVectAScan->getParamLGate0Width())<<";";
                flux<</*m->seuil1Pic*/pVectAScan->getParamLGate0Height()<<";";

                // convert US-SPI signal to US-KEY range
                for(int j=0;j<AScanRawSample::ASCAN_SAMPLE_SIZE;j++)
                {
                    qreal d = pAScan->getAScanRawSample()->m_rawSamples[j];
                    qreal uskeyData = 2048 + ((d-127)*10);
                    flux<<(unsigned short)(uskeyData)<<";";
                }

                flux << i->getTemperatureStr() << ";";
                flux << i->getHumidityStr() << ";";

                //flux<<m->deepCm<<";"; // IP68
                flux<<"\n";
            }

            progress++;
            qint32 percent = 100 * progress / pCell->size();
            if( percent != lastPrecent ) {
                lastPrecent = percent;
                emit evProgessChanged(percent);
            }

            if( m_requestCancel ) {
                bOk = false;
                break;
            }
        }
        if (flux.status() != QTextStream::Ok)
        {
           bOk = false;
           fLogError(QString("Fail to write CSV File %1").arg(m_fileName));
        }

        file.close();
    } else {
        fLogError("Fail to create report file : " + m_fileName);
        bOk = false;
    }

//    for(int i=0; i<= 100; i++) {
//        emit evProgessChanged(i);
//        QThread::msleep(20);
//        if( m_requestCancel ) {
//            bOk = false;
//            break;
//        }
//    }

    if( !bOk ) {
         emit evProgessChanged(-1);
    }

    emit evExportTerminated(bOk);
}

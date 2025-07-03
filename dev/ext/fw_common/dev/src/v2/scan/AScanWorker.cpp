/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : AScan.cpp
# Author      : Thierry Lechevallier
# Date        : 26/07/2021
#
# ==========================================================================*/


// -- Internal includes
#include "AScanWorker.h"
#include <v2/vector/VectorFuncAScan.h>
// -- Qt Includes
#include <QtMath>

AScanWorker::AScanWorker() : Loggable("AScanWorker")
{
    m_isBusy = false;
    m_clearPipe = 0;
    m_pThread = new QThread();

    connect(this, &AScanWorker::evPrivateTreatAScan, this, &AScanWorker::onPrivateTreatAScan, Qt::ConnectionType(Qt::UniqueConnection | Qt::QueuedConnection));

    this->moveToThread(m_pThread);
    m_pThread->start(QThread::HighPriority);
}

AScanWorker::~AScanWorker()
{    
    clearAScan();
    disconnect(this, &AScanWorker::evPrivateTreatAScan, this, &AScanWorker::onPrivateTreatAScan);
    if( m_pThread->isRunning() ) {
        m_pThread->quit();
        m_pThread->wait(500);
    }
    if( m_pThread->isRunning() ) {
        m_pThread->terminate();
    }
    delete m_pThread;
    m_pThread = nullptr;

}

bool AScanWorker::isBusy()
{
    return m_isBusy;
}

void AScanWorker::pushAScanToTreat(AScan * p_pAScan)
{
    if( m_clearPipe & 0b10 ) {
        delete p_pAScan;
        m_clearPipe &= 0b01;  // clear bit 2
    } else {
        m_lAScan.lock();
        m_lAscanToTreat.append(p_pAScan);
        if( m_lAscanToTreat.size() > VectorFuncAScan::ASCAN_BUFFER_SIZE_MAX) {
            delete m_lAscanToTreat.takeFirst();
        }
        m_lAScan.unlock();
        if( !m_isBusy ) {
            m_isBusy = true;
            emit evPrivateTreatAScan();
        }
    }

}

AScan* AScanWorker::takeAScanTreated()
{
    AScan* pAscan = nullptr;
    if( m_lAscanTreated.size() ) {
        pAscan = m_lAscanTreated.takeFirst();
    }
    return pAscan;
}

bool AScanWorker::hasAscanTreated()
{
    bool bRet = false;
    m_lAScan.lock();
    bRet = m_lAscanTreated.size();
    m_lAScan.unlock();
    return bRet;
}

void AScanWorker::clearAScan()
{
    disconnect(this, &AScanWorker::evPrivateTreatAScan, this, &AScanWorker::onPrivateTreatAScan);
    if( m_isBusy ) {
        m_clearPipe = 0b11;
    } else {
        m_clearPipe = 0b10;
    }
    m_lAScan.lock();
    while(m_lAscanToTreat.size() ) {
        delete m_lAscanToTreat.takeLast();
    }
    while(m_lAscanTreated.size() ) {
        delete m_lAscanTreated.takeLast();
    }
    m_lAScan.unlock();
    connect(this, &AScanWorker::evPrivateTreatAScan, this, &AScanWorker::onPrivateTreatAScan, Qt::ConnectionType(Qt::UniqueConnection | Qt::QueuedConnection));
}

void AScanWorker::onPrivateTreatAScan()
{
    m_pAScanTreated = nullptr;

    m_lAScan.lock();
    if( m_lAscanToTreat.isEmpty() ) {
        m_isBusy = false;
    } else {
        m_pAScanTreated = m_lAscanToTreat.takeFirst();
    }
    m_lAScan.unlock();

    if( nullptr != m_pAScanTreated ) {
        computeEnhancedAScan();
        computeMeasures();
        if( m_clearPipe ) {
            delete m_pAScanTreated;
        } else {
            m_lAscanTreated.append(m_pAScanTreated);
            emit evAScanTreated();
        }
        if( m_lAscanToTreat.size() ) {
            emit evPrivateTreatAScan();
        } else {
            m_isBusy = false;
        }
    }

    m_clearPipe &= 0b10;  // clear bit 1
}

qreal dSinc(double x)
{
    if( x == 0 ) {
        return 1;
    } else {
        return qSin(x)/x;
    }
}

void AScanWorker::computeEnhancedAScan()
{
//
//    // for now, just copy datas
//    memcpy(m_pAScanTreated->getAScanEnhancedSample()->m_enhancedSamples,
//           m_pAScanTreated->getAScanRawSample()->m_rawSamples,
//           AScanRawSample::ASCAN_SAMPLE_SIZE);

    // -- Function imported from project <<DATASCAN 4.x>> (AScanFilter.cpp)
    // constants
    static const qreal frequenceCoupure = 800000;

    // data
    quint8 *entree = m_pAScanTreated->getAScanRawSample()->m_rawSamples;
    quint8 *sortie = m_pAScanTreated->getAScanEnhancedSample()->m_enhancedSamples;

    if(m_pAScanTreated->getAScanEnhancedSample()->m_paramLLowPassFilter>5)
    {
        int attenuation_locale = m_pAScanTreated->getAScanEnhancedSample()->m_paramLLowPassFilter;
        if(attenuation_locale%2 != 0) attenuation_locale++;
        double coeff[attenuation_locale];

        qreal sampFreq = m_pAScanTreated->getSensorSettings()->getSampleFrequency();
        for(int i=0;i<attenuation_locale;i++)
        {
            double sincFilter = (2.* frequenceCoupure/sampFreq) *dSinc(2.*frequenceCoupure*(i-((attenuation_locale-1.)/2.))/sampFreq);
            coeff[i] = qAbs((0.42 - 0.5*qCos((2.*M_PI*i) / (attenuation_locale-1.))+ 0.08*qCos((4.*M_PI *i)/(attenuation_locale-1.))) * sincFilter);
        }

        double somme = 0;

        for(int i=0;i<AScanRawSample::ASCAN_SAMPLE_SIZE-attenuation_locale-1;i++)
        {
            somme = 0.;
            for(int j=0;j<attenuation_locale-1;j++)
            {                
                qreal entre = (abs(entree[j+i]-127)*2);
                somme += ((coeff[attenuation_locale-j]) * entre*10);
            }

//                if( somme > 1000 ) {
//                    fLogInfo(QString("somme [%1] = %2").arg(i).arg(somme));
//                }
            sortie[i]=qMin((qreal)255,somme);

        }
        for(int i=AScanRawSample::ASCAN_SAMPLE_SIZE-attenuation_locale-1;i<AScanRawSample::ASCAN_SAMPLE_SIZE;i++)
        {
            sortie[i]=0;
        }
    }else {
        for(int i=0;i<AScanRawSample::ASCAN_SAMPLE_SIZE;i++)
        {
            sortie[i]=qMin(255,(abs(entree[i]-127)*2));
        }
    }
}

void AScanWorker::computeMeasures()
{
    // for now, just put value
//    m_pAScanTreated->getAScanMeasure()->m_thicknessMmMultiEcho = m_pAScanTreated->getAScanRawSample()->m_rawSamples[0]/10;
//    m_pAScanTreated->getAScanMeasure()->m_thicknessMmMultiEcho = m_pAScanTreated->getAScanMeasure()->m_thicknessMmMultiEcho *1.1;
//    m_pAScanTreated->getAScanMeasure()->m_thicknessMm1Echo= m_pAScanTreated->getAScanRawSample()->m_rawSamples[0]/11;



    // -- Function imported from project <<DATASCAN 4.x>> (calculmesure.cpp : cherchePics)

    static const qreal distanceMiniEntrePic=50;
    static const qint32 precisionMesure = 0;
    qreal niveauBruit =0.;

    qint32 debut1Pic = m_pAScanTreated->mmToIndex(m_pAScanTreated->getAScanMeasure()->m_paramGates0->m_paramLStart);
    qint32 fin1Pic = m_pAScanTreated->mmToIndex(m_pAScanTreated->getAScanMeasure()->m_paramGates0->m_paramLStart+m_pAScanTreated->getAScanMeasure()->m_paramGates0->m_paramLWidth);
    // m_paramLHeight is in % => x2.55 to convert to ASCAN scale (8 bit : 0..255)
    qreal seuil1Pic = m_pAScanTreated->getAScanMeasure()->m_paramGates0->m_paramLHeight*2.55;

    qint32 debutSignal = m_pAScanTreated->mmToIndex(m_pAScanTreated->getAScanMeasure()->m_paramGates1->m_paramLStart);
    qint32 finSignal = m_pAScanTreated->mmToIndex(m_pAScanTreated->getAScanMeasure()->m_paramGates1->m_paramLStart+m_pAScanTreated->getAScanMeasure()->m_paramGates1->m_paramLWidth);
    // m_paramLHeight is in % => x2.55 to convert to ASCAN scale (8 bit : 0..255)
    qreal seuilBruit = m_pAScanTreated->getAScanMeasure()->m_paramGates1->m_paramLHeight*2.55;

    QList<quint32> & listePics = m_pAScanTreated->getAScanMeasure()->m_listPicsIndex;

    quint8 *signal = m_pAScanTreated->getAScanEnhancedSample()->m_enhancedSamples;

    //On estime que le bruit est le même partout sur le signal donc on recherche celui-ci au début de l'acquisition (juste après le pic d'émission du capteur) jusqu'à la porte du premier pic
    //On définit le bruit comme le maximum observé sur cette portion du signal.
    for(int i=200;i<debut1Pic;i++)
    {
        if(signal[i]>niveauBruit)
        {
            niveauBruit = signal[i];
        }
    }

    //On va maintenant chercher le premier pic.
    //Un pic est définit comme étant un point où le précédant ET le suivant lui sont inférieur.
    //Une source d'erreur ici est qu'on prend le premier trouvé et non le maximum.
    int indexPremierEcho = 0;
    int indexPremierEchoValue = 0;
    for(int i=debut1Pic;i<fin1Pic;i++)
    {
        if(signal[i]>signal[i-1] && signal[i]>=signal[i+1] && signal[i] > seuil1Pic && signal[i] > indexPremierEchoValue)
        {
            indexPremierEcho = i;
            indexPremierEchoValue = signal[i];
        }
        // Continue to find a max only if not found, or found but still over thresold Gate1
        if( signal[i] < seuil1Pic && indexPremierEcho != 0 ) {
            // Do not find max anymore
            break;
        }
    }
    if(indexPremierEcho != 0)
        //Si un premier pic est trouvé alors on l'ajoute dans une liste et on recherche tous les suivants après.
    {
        listePics.append(indexPremierEcho);
    }

    if(listePics.size() > 0)
        //On cherche tous les pics présents dans le signal :
    {
        qint32 indexListe = 1;
        for(int i = debutSignal +1; i<finSignal-1;i++)
        {
            if(signal[i] > signal[i - 1] && signal[i] >= signal[i+1] &&
                    signal[i] > seuilBruit && ((i- listePics.at(indexListe-1) > distanceMiniEntrePic) || signal[i]>signal[listePics.at(indexListe-1)]))
                //On cherche tous les pics à condition qu'il soit séparé d'une distance minimale. Cependant, on peut accepter un pic proche d'un autre s'il est plus grand que le précédant.
            {
                if(indexListe>2)
                {
                    if(i-listePics.at(indexListe-1) < distanceMiniEntrePic && signal[i] > signal[listePics.at(indexListe-1)])
                        //Si plusieurs pics sont proches les uns des autres, on ne garde que le plus grand d'entre eux.
                    {
                        listePics.removeAt(listePics.size()-1);
                        indexListe--;
                    }
                }

                listePics.append(i);
                indexListe++;

                if(qAbs(listePics.at(indexListe-1) - listePics.at(indexListe-2)) < distanceMiniEntrePic)
                {
                    if(listePics.at(indexListe-1) < listePics.at(indexListe-2))
                    {
                        listePics.removeAt(indexListe-1);
                    }else
                    {
                        listePics.removeAt(indexListe-2);
                    }
                    indexListe--;
                }
            }
        }
    }

    // -- Function imported from project <<DATASCAN 4.x>> (calculmesure.cpp : calculDistance)


    qint32 compteurPics = listePics.size();
    if (compteurPics > 2 )
    {
        for(int i=1; i< compteurPics -2;i++)
        {
            if((signal[listePics.at(i)] < signal[listePics.at(i-1)]
                    && signal[listePics.at(i)] < signal[listePics.at(i+1)]
                    && abs(qint32((listePics.at(i+1) - listePics.at(i)) - (listePics.at(i)-listePics.at(i-1)))) >= 50)
                    || (signal[listePics.at(i)] < signal[listePics.at(i-1)]*0.5
                    && signal[listePics.at(i)] < signal[listePics.at(i+1)]*0.8))
            {
                listePics.removeAt(i);
                compteurPics--;
            }
        }
    }
    //Deux fois la même boucle
    compteurPics = listePics.size();

    if (compteurPics > 2 )
    {
        for(int i=1; i< compteurPics -2;i++)
        {
            if((signal[listePics.at(i)] < signal[listePics.at(i-1)]
                    && signal[listePics.at(i)] < signal[listePics.at(i+1)]
                    && abs(qint32((listePics.at(i+1) - listePics.at(i)) - (listePics.at(i)-listePics.at(i-1)))) >= precisionMesure)
                    || (signal[listePics.at(i)] < signal[listePics.at(i-1)]*0.5
                    && signal[listePics.at(i)] < signal[listePics.at(i+1)]*0.8))
            {
                listePics.removeAt(i);
                compteurPics--;
            }
        }
    }


    QList<unsigned short int> listeDistance;

    if(listePics.size()<100)
    {
        double forceSignal[100];
        double moyenne[100];
        int poids[100];
        double forceSignalMesure[100];

        for(int i=0;i<listePics.size()-1;i++)
        {
            listeDistance.append(listePics.at(i+1) - listePics.at(i));
            forceSignal[i] = signal[listePics.at(i+1)] + signal[listePics.at(i)];
        }

        qint32 recurenceMaxMesure = 0;
        for(int i=0; i<listePics.size()-1;i++)
        {
            int reference = listeDistance.at(i);
            int somme = 0;
            int nombre = 0;
            moyenne[i] = 0;
            forceSignalMesure[i] = 0;
            if(listeDistance.at(i) > 0)
            {
                for(int j=0;j<listePics.size()-1;j++)
                {
                    if(abs(listeDistance.at(j) - reference) <4)
                    {
                        somme += listeDistance.at(j);
                        forceSignalMesure[i] += forceSignal[i];
                        nombre++;
                    }
                }
            }
            poids[i] = nombre;
            if(nombre!=0)
            {
                moyenne[i] = somme / nombre;
            }
            if(nombre > recurenceMaxMesure)
            {
                recurenceMaxMesure = nombre;
            }
        }

        qreal mesureMultiEcho;
        qreal mesure1Echo;

        if(recurenceMaxMesure < 1)
        {
            mesureMultiEcho = -1;
            mesure1Echo = -1;
        }else
        {
            int indexMesure = 0;
            for(int i = 1; i<listePics.size()-1; i++)
            {
                if(forceSignalMesure[i] * poids[i] > forceSignalMesure[indexMesure]*poids[indexMesure])
                {
                    indexMesure = i;
                }
            }
            mesureMultiEcho = moyenne[indexMesure];

        }
        if(mesureMultiEcho == -1 && listePics.size() == 1)
        {
            mesureMultiEcho = 0;
            mesure1Echo = 0;

        }else if (listePics.size() > 1 && mesureMultiEcho!=-1)
        {
            mesureMultiEcho *= 1000. * m_pAScanTreated->getSensorSettings()->m_paramLCelerity / (m_pAScanTreated->getSensorSettings()->getSampleFrequency() *2);
        }
        if(listePics.size()>0)
        {
            // "dephasage
            qreal dephasage = m_pAScanTreated->getAScanMeasure()->m_paramLProbeDelayMm;
            mesure1Echo = (double)(listePics.at(0)) * 1000. * m_pAScanTreated->getSensorSettings()->m_paramLCelerity / (m_pAScanTreated->getSensorSettings()->getSampleFrequency() *2) - dephasage;
        }else
        {
            mesure1Echo = 0.;
        }
        if(mesureMultiEcho == -1)
        {
            mesureMultiEcho = 0;
        }

        m_pAScanTreated->getAScanMeasure()->m_thicknessMmMultiEcho = mesureMultiEcho;
        m_pAScanTreated->getAScanMeasure()->m_thicknessMm1Echo = mesure1Echo;
    }
}
